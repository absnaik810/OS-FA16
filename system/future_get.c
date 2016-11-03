#include<xinu.h>
#include<future.h>
#include<stdio.h>

syscall future_get(future *fut, int *value) {
	if(fut->flag==FUTURE_EXCLUSIVE) {
		if(fut->state == FUTURE_EMPTY) {
			fut->state = FUTURE_WAITING;
			fut->pid = getpid();
			suspend(fut->pid);

			return OK;		
		} else if(fut->state == FUTURE_VALID) {		
			*value = *(fut->value);
			resume(fut->pid);
			fut->state = FUTURE_EMPTY;

			return OK;
	} else {
			return SYSERR;
	}
} 
}