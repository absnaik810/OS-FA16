/* clkhandler.c - clkhandler */

#include <xinu.h>

void	clkhandler()
{

	static uint32 counter = 1000;	
	volatile struct am335x_timer1ms *csrptr = 0x44E31000;

	if((csrptr->tisr & AM335X_TIMER1MS_TISR_OVF_IT_FLAG) == 0) {
		return;
	}

	csrptr->tisr = AM335X_TIMER1MS_TISR_OVF_IT_FLAG;

	counter--;

	if(counter == 0) {
		clktime++;
		counter = 1000;
		arp_cache_clear();
	}

	if(!isempty(sleepq)) {

		if((--queuetab[firstid(sleepq)].qkey) == 0) {
			//Calling wakeup()
			wakeup();
		}
	}
	
	if((--preempt) == 0) {
		preempt = QUANTUM;
		
		//Calling resched()
		resched();
	}
}