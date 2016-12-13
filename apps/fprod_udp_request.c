#include "future.h"

int fprod_udp_request(future *fut, int serverip, int portno, int locport)
{
	char buf[512];
	char msg[512];
	int retval, msg_len, slot;
	int val, status;
	memset(msg,'\0',sizeof(msg));
	getchar();
	slot = udp_register(serverip, portno, locport);
	if (slot == SYSERR) 	
	{
		printf("Error while calling the UDP slot %d ",locport);
		return -1;
	}
	while(1)
	{
		printf("\nTesting...");
		while(fut->state==FUTURE_VALID)
			printf("");
		printf("\n>");
		
		msg_len = strlen(msg);
		msg[msg_len-1]='\0';
		kprintf("\nTesting: %d %s",msg_len,msg);
		
		retval = udp_send(slot, msg, msg_len);
		if(retval == SYSERR)
		{
			printf(" Error in udp sending datagram ");
			return -1;
		}
	
		retval = udp_recv(slot, buf, sizeof(buf),3000);
		
		if(retval == TIMEOUT)
		{
			printf("Timeout in receiving the datagram");
			return -1;
		}
		
		if(retval == SYSERR)
		{ 	
			printf("Error in receiving the datagram");
			return -1;
		}
		
		val = atoi(buf);
		
  		if (status < 1)
  		{
    			printf("Future_set failed\n");
    			return -1;
 		}
		if(val==-1)
			break;
		memset(buf,'\0',sizeof(buf));
		
	}
	udp_release(slot);
	
	return 0;
}