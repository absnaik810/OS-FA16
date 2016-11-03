#include<xinu.h>
#include<future.h>

syscall future_free(future* fut) {	
	freemem(fut, sizeof(fut));
	printf("Memory for future is released \n"); 
	return OK;
} 