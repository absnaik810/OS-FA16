#include<xinu.h>
#include<future.h>

future* future_alloc(int future_flags) {
	
	future* fut;
	//allocate memory for the future by calling getmem
	fut = (future *)getmem(sizeof(* fut));
	//set the appropriate future flag
	fut->flag = future_flags;
	//set the appropriate future state
	fut->state = FUTURE_EMPTY;

	return fut;
}  