#include<xinu.h>
#include<future.h>

syscall future_free(future* fut) {	
	//code to free the memory
	freemem(fut, sizeof(fut));
	return OK;
} 