#include <xinu.h>
#include <stdio.h>
#include <string.h>

static	void	arp_dmp();

shellcmd xsh_arp_test(int narg, char *args[])
{

	if (narg == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tIt displays the information from the ARP cache\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit.\n");
		return 0;
	}
	
	int32 slot;
	for(slot = 0; slot < ARP_SIZ; slot++){
		if(arpcache[slot].arstate == AR_FREE){
			arpcache[slot].arstate = AR_RESOLVED;
			arpcache[slot].arpaddr = 192.168;
			arpcache[slot].arpid   = 1;
			arpcache[slot].arhaddr[0] = 1111 ;
			arpcache[slot].timestamp = clktime;
			break;
		}
	}

	printf("\n");
	arp_dmp();

	return 0;
}

static	void arp_dmp ()
{
	int32	i, j;			
	struct	arpentry *arptr;	

	printf("ARP cache:\n");
	printf("   State Pid    IP Address    Hardware Address	Timestamp\n");
	printf("   ----- --- --------------- -----------------\n");
	for (i = 0; i < ARP_SIZ; i++) {
		arptr = &arpcache[i];
		if (arptr->arstate == AR_FREE) {
			continue;
		}
		switch(arptr->arstate) {
		    case AR_PENDING:	printf("   PEND "); break;
		    case AR_RESOLVED:	printf("   RESLV"); break;
		    default:		printf("   ?????"); break;
		}
		if (arptr->arstate == AR_PENDING) {
			printf("%4d ", arptr->arpid);
		} else {
			printf("     ");
		}
		printf("%3d.", (arptr->arpaddr & 0xFF000000) >> 24);
		printf("%3d.", (arptr->arpaddr & 0x00FF0000) >> 16);
		printf("%3d.", (arptr->arpaddr & 0x0000FF00) >> 8);
		printf("%3d",  (arptr->arpaddr & 0x000000FF));

		printf(" %02X", arptr->arhaddr[0]);
		for (j = 1; j < ARP_HALEN; j++) {
			printf(":%02X", arptr->arhaddr[j]);
		}
		printf("     ");
		printf("%d", arptr->timestamp);
		printf("\n");
	}
	printf("\n");
	return;
}