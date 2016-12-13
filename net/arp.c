#include <xinu.h>

struct	arpentry  arpcache[ARP_SIZ];

void	arp_init(void)
{
	int32	i;			

	for (i=1; i<ARP_SIZ; i++) {	
		arpcache[i].arstate = AR_FREE;
	}
}

status	arp_resolve (
	 uint32	nxthop,			
	 byte	mac[ETH_ADDR_LEN]	
	)				
{
	intmask	mask;			
	struct	arppacket apkt;		
	int32	i;			
	int32	slot;			
	struct	arpentry  *arptr;	
	int32	msg;			

	if (nxthop == IP_BCAST) {
		memcpy(mac, NetData.ethbcast, ETH_ADDR_LEN);
		return OK;
	}

	if (nxthop == NetData.ipbcast) {
		memcpy(mac, NetData.ethbcast, ETH_ADDR_LEN);
		return OK;
	}

	mask = disable();

	for (i=0; i<ARP_SIZ; i++) {
		arptr = &arpcache[i];
		if (arptr->arstate == AR_FREE) {
			continue;
		}
		if (arptr->arpaddr == nxthop) { 
			break;
		}
	}

	if (i < ARP_SIZ) {	

		if (arptr->arstate == AR_RESOLVED) {
			memcpy(mac, arptr->arhaddr, ARP_HALEN);
			arptr->timestamp = clktime;
			restore(mask);
			return OK;
		}

		if (arptr->arstate == AR_PENDING) {
			restore(mask);
			return SYSERR;
		}
	}

	slot = arp_alloc();
	if (slot == SYSERR) {
		restore(mask);
		return SYSERR;
	}

	arptr = &arpcache[slot];
	arptr->arstate = AR_PENDING;
	arptr->arpaddr = nxthop;
	arptr->arpid = currpid;

	memcpy(apkt.arp_ethdst, NetData.ethbcast, ETH_ADDR_LEN);
	memcpy(apkt.arp_ethsrc, NetData.ethucast, ETH_ADDR_LEN);
	apkt.arp_ethtype = ETH_ARP;	  
	apkt.arp_htype = ARP_HTYPE;	  
	apkt.arp_ptype = ARP_PTYPE;	  
	apkt.arp_hlen = 0xff & ARP_HALEN; 
	apkt.arp_plen = 0xff & ARP_PALEN; 
	apkt.arp_op = 0xffff & ARP_OP_REQ;
	memcpy(apkt.arp_sndha, NetData.ethucast, ARP_HALEN);
	apkt.arp_sndpa = NetData.ipucast; 
	memset(apkt.arp_tarha, '\0', ARP_HALEN); 
	apkt.arp_tarpa = nxthop;	  

	arp_hton(&apkt);

	eth_hton((struct netpacket *)&apkt);

	msg = recvclr();
	for (i=0; i<ARP_RETRY; i++) {
		write(ETHER0, (char *)&apkt, sizeof(struct arppacket));
		msg = recvtime(ARP_TIMEOUT);
		if (msg == TIMEOUT) {
			continue;
		} else if (msg == SYSERR) {
			restore(mask);
			return SYSERR;
		} else {	
			break;
 		}
	}

	if (msg == TIMEOUT) {
		arptr->arstate = AR_FREE;   
		restore(mask);
		return TIMEOUT;
	}

	memcpy(mac, arptr->arhaddr, ARP_HALEN);
	restore(mask);
	return OK;
}

void	arp_in (
	  struct arppacket *pktptr	
	)
{
	intmask	mask;			
	struct	arppacket apkt;		
	int32	slot;			
	struct	arpentry  *arptr;	
	bool8	found;			
	
	arp_ntoh(pktptr);

	if ( (pktptr->arp_htype != ARP_HTYPE) ||
	     (pktptr->arp_ptype != ARP_PTYPE) ) {
		freebuf((char *)pktptr);
		return;
	}

	mask = disable();

	found = FALSE;

	for (slot=0; slot < ARP_SIZ; slot++) {
		arptr = &arpcache[slot];

		if (arptr->arstate == AR_FREE) {
			continue;
		}

		if (arptr->arpaddr == pktptr->arp_sndpa) {
			found = TRUE;
			break;
		}
	}

	if (found) {

		memcpy(arptr->arhaddr, pktptr->arp_sndha, ARP_HALEN);

		if (arptr->arstate == AR_PENDING) {
			arptr->arstate = AR_RESOLVED;
			arptr->timestamp = clktime;
			send(arptr->arpid, OK);
		}
	}

	if ((!NetData.ipvalid) ||
			(pktptr->arp_tarpa != NetData.ipucast)) {
		freebuf((char *)pktptr);
		restore(mask);
		return;
	}
	
	if (pktptr->arp_op == ARP_OP_RPLY) {
		freebuf((char *)pktptr);
		restore(mask);
		return;
	}

	if (!found) {
		slot = arp_alloc();
		if (slot == SYSERR) {	
			kprintf("ARP cache overflow on interface\n");
			freebuf((char *)pktptr);
			restore(mask);
			return;
		}
		arptr = &arpcache[slot];
		arptr->arpaddr = pktptr->arp_sndpa;
		memcpy(arptr->arhaddr, pktptr->arp_sndha, ARP_HALEN);
		arptr->arstate = AR_RESOLVED;
	}

	memcpy(apkt.arp_ethdst, pktptr->arp_sndha, ARP_HALEN);
	memcpy(apkt.arp_ethsrc, NetData.ethucast, ARP_HALEN);
	apkt.arp_ethtype= ETH_ARP;		
	apkt.arp_htype	= ARP_HTYPE;		
	apkt.arp_ptype	= ARP_PTYPE;		
	apkt.arp_hlen	= ARP_HALEN;		
	apkt.arp_plen	= ARP_PALEN;		
	apkt.arp_op	= ARP_OP_RPLY;		

	memcpy(apkt.arp_sndha, NetData.ethucast, ARP_HALEN);
	apkt.arp_sndpa = NetData.ipucast;

	memcpy(apkt.arp_tarha, pktptr->arp_sndha, ARP_HALEN);
	apkt.arp_tarpa = pktptr->arp_sndpa;
	arp_hton(&apkt);
	eth_hton((struct netpacket *)&apkt);
	write(ETHER0, (char *)&apkt, sizeof(struct arppacket));
	freebuf((char *)pktptr);
	restore(mask);
	return;
}

int32	arp_alloc ()
{
	int32	slot;			

	for (slot=0; slot < ARP_SIZ; slot++) {
		if (arpcache[slot].arstate == AR_FREE) {
			memset((char *)&arpcache[slot],
					NULLCH, sizeof(struct arpentry));
			return slot;
		}
	}

	for (slot=0; slot < ARP_SIZ; slot++) {
		if (arpcache[slot].arstate == AR_RESOLVED) {
			memset((char *)&arpcache[slot],
					NULLCH, sizeof(struct arpentry));
			return slot;
		}
	}

	kprintf("ARP cache size exceeded\n");

	return SYSERR;
}

int32 arp_cache_clear()
{
	
	int32	slot;
	intmask	mask;			
	mask = disable();
	for(slot=0; slot < ARP_SIZ; slot++){
		if(arpcache[slot].arstate == AR_RESOLVED){
			
			if((clktime - arpcache[slot].timestamp) > 300){
			
			arpcache[slot].arstate = AR_FREE;
			arpcache[slot].arpaddr = 0;
			arpcache[slot].arpid   = -1;
			memset(&arpcache[slot].arhaddr, NULLCH, ARP_HALEN*sizeof(byte));
			arpcache[slot].timestamp = 0;
			}	
		}
	}	
	restore(mask);
	return 0;
}

void 	arp_ntoh(
	struct arppacket *pktptr
	)
{
	pktptr->arp_htype = ntohs(pktptr->arp_htype);
	pktptr->arp_ptype = ntohs(pktptr->arp_ptype);
	pktptr->arp_op    = ntohs(pktptr->arp_op);
	pktptr->arp_sndpa = ntohl(pktptr->arp_sndpa);
	pktptr->arp_tarpa = ntohl(pktptr->arp_tarpa);
}

void 	arp_hton(
	struct arppacket *pktptr
	)
{
	pktptr->arp_htype = htons(pktptr->arp_htype);
	pktptr->arp_ptype = htons(pktptr->arp_ptype);
	pktptr->arp_op    = htons(pktptr->arp_op);
	pktptr->arp_sndpa = htonl(pktptr->arp_sndpa);
	pktptr->arp_tarpa = htonl(pktptr->arp_tarpa);
}