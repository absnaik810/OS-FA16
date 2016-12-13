// #include <xinu.h>
#include "future.h"

int fcons_udp_print(future *fut)
{
	int i, status;
	while(1)
	{
		while(fut->state==FUTURE_EMPTY || fut->state==FUTURE_WAITING)
			printf("");

		status = future_get(fut, &i);

		if(i==-1)
			break;
		
		if (status < 1)
	  	{
	    		printf("future_get failed\n");
	    		return -1;
	  	}

		kprintf("\nConsumer consumed %d", i);
	}	
	if(!future_free(fut));
		return -1;
		
	return 0;
}