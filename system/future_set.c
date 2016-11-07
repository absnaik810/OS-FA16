#include<xinu.h>
#include<future.h>

syscall future_set(future *fut, int *value) {
//checking if the future flag is exclusive
if(fut->flag==FUTURE_EXCLUSIVE) {	
	//check the states of the futures
	if(fut->state == FUTURE_WAITING || fut->state == FUTURE_EMPTY) {	
			//set the necessary future properties
			fut->state = FUTURE_VALID;
			fut->value = *value;
			resume(fut->pid);

			return OK;
		} else  {			

			return SYSERR;
		} 
	}
} 