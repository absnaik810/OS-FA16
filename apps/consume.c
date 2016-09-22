/* consume.c - contains the definition of the consume function */

#include <xinu.h>
#include <prodcons.h>

/*-----------------------------------------------------------------
 * consume - consumes the values of 'n' until it is less than count 
 * and then prints it.
 *---------------------------------------------------------------*/

void consumer(int count) 
{
	int32 i;

	for(i=0; i<count; i++)
	{
		printf("consumed : %d\n", n);
	}
}

