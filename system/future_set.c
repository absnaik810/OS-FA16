/* Used for implementing the future_set() function */

#include<future.h>
#include<xinu.h>

syscall future_set(future* fut, int* value) {
	if(fut->state==FUTURE_EMPTY) {
		fut->value=value;
		fut->state=FUTURE_VALID;
		return OK;
	}
	else {
		if(fut->state==FUTURE_VALID) {
			return SYSERR;
	        }
		else {
			resume(fut->pid);	
		}
	}
}
