#include "prodcons.h"
#include "future.h"
#include <stdlib.h>
#include <string.h>

shellcmd xsh_udp_request(int narg, char *args[])
{

	int  portno, retval, fpid, temp;
	char buf[512];
	char msg[512];
	uint32 serverip;	
	int msg_len, free_status;		
	int locport = 42000;
	int slot;		
	
	int future_flag;
	future_flag=0;
	future *f_exlusive;
	int val,status;

	if(narg==4 && strcmp(args[3],"-f")==0)
	{
		//goto futures;
		future_flag = 1;
	}

	if(narg > 4 || (narg > 3 && strcmp(args[3],"-f")!=0 ))
	{
		printf("Too many arguments for the udp_request command.  Please enter less commands.\n");
		printf("Use --help to know how to use.\n");
		return -1;
	}	

	if(narg==3 && strcmp(args[2],"-f")==0)
	{
		printf("\nToo few arguments for future mode.\n");
		return -1;
	}
	
	if(narg < 3)
	{
		printf("Too few arguments for the udp_request command.\n");
		printf("Use --help to know about usage.\n");
		return -1;
	}

	if((narg == 2) && strncmp(args[1],"--help",7)== 0)
	{
		
		printf("Use: %s Find length of string\n\n");
		printf("Description:\n");
		printf("\tCommunicate with server using UDP.\n");
		printf("Options:\n");
		printf("\tServerip:\tIP address of the server:\n");
		printf("\tPortno:\tPort number of the server:\n");
		printf("\tServerip:\tNumber of which square needs to be computed: \n");		
		printf("\t-f Use future mode: \n");
		printf("\t--help\t display this help and exit:\n");
		return -1;
	}

	if (dot2ip(args[1], &serverip) == SYSERR) {
		printf( "%s: invalid IP address argument\r\n",
			args[0]);
		return 1;
	}
	produced1 = semcreate(1);
	
	portno = atoi(args[2]);

	//strncpy(msg,args[3],strlen(args[3]));
	slot = udp_register(serverip, portno, locport);		// register a slot in udp table
	if (slot == SYSERR) 	
	{
		printf(" Error while calling udp register %d ",locport);
		return -1;
	}

	if (!future_flag)	
	{
		while(1)
		{
			printf("\n>");
			fgets(msg,512,CONSOLE);
			msg_len = strlen(msg);
			msg[msg_len-1]='\0';
			if(strcmp(msg,"exit") == 0) 
				break;

			retval = udp_send(slot, msg, msg_len);
			if(retval == SYSERR)
			{
				printf(" Error in UDP sending datagram ");
				return -1;
	
			}
	
			retval = udp_recv(slot, buf, sizeof(buf),3000);
			if(retval == SYSERR)
			{ 	
				printf("Error in receiving datagram");
				return -1;

			}

			if(retval == TIMEOUT)
			{
				printf("Timeout in receiving datagram");
				return -1;
			}

			printf("Length of %s is  %s \n",msg,buf);
			memset(buf,'\0',sizeof(buf));
			memset(msg,'\0',sizeof(msg));
		}
		udp_release(slot);
	}	
	else
	{	
		f_exlusive = future_alloc(FUTURE_EXCLUSIVE);

		if (f_exlusive != NULL )
		{
			fpid = create(fcons_udp_print, 1024, 20, "fcons1", 1, f_exlusive);
			resume(fpid);
	
			while(1)
			{
				while(f_exlusive->state==FUTURE_VALID)
					printf("");

				printf("\n>");
				fgets(msg,512,CONSOLE);
				msg_len = strlen(msg);
				msg[msg_len-1]='\0';

				if(strcmp(msg,"exit") == 0) 
					break;
				retval = udp_send(slot, msg, msg_len);
				if(retval == SYSERR)
				{
					printf(" Error in udp sending datagram ");
					return -1;
				}
	
				retval = udp_recv(slot, buf, sizeof(buf),3000);
				if(retval == SYSERR)
				{ 	
					printf("Error in receiving datagram");
					return -1;
				}
				if(retval == TIMEOUT)
				{
					printf("Timeout in receiving datagram");
					return -1;
				}
				val = atoi(buf);
				printf("\nProducer produced %d",val);
				status = future_set(f_exlusive, &val);
  				if (status < 1)
  				{
    					printf("future_set failed\n");
    					return -1;
 				}
				if(val==-1)
					break;
				memset(buf,'\0',sizeof(buf));
				memset(msg,'\0',sizeof(msg));
		
			}
		}
		udp_release(slot);
		while(count)
			printf("");

		if(!(future_free(f_exlusive)))
			return SYSERR;


	}	
	return 0;

}
