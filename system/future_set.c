#include<xinu.h>
#include<future.h>

syscall future_set(future *fut, int *value) {
if(fut->flag==FUTURE_EXCLUSIVE) {	
	if(fut->state == FUTURE_WAITING || fut->state == FUTURE_EMPTY) {	
			fut->state = FUTURE_VALID;
			fut->value = *value;
			resume(fut->pid);

			return OK;
		} else  {			

			return SYSERR;
		} 
	}
}