#include<xinu.h>
#include<future.h>
#include<stdio.h>

syscall future_get(future *fut, int *value) {
	//check the flag to see if it is future exclusive
	if(fut->flag==FUTURE_EXCLUSIVE) {
		//check the state of the future
		if(fut->state == FUTURE_EMPTY) {
			//setting the necessary future properties if the state is empty
			fut->state = FUTURE_WAITING;
			fut->pid = getpid();
			suspend(fut->pid);

			return OK;		
		} else if(fut->state == FUTURE_VALID) {		
			//setting the necessary future properties if the state is empty
			*value = *(fut->value);
			resume(fut->pid);
			fut->state = FUTURE_EMPTY;

			return OK;
	} else {
			return SYSERR;
	}
} 
}