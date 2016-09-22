/* produce.c - includes the definition of the producer function */

#include <xinu.h>
#include <prodcons.h>

/*--------------------------------------------------------------------
 * producer produces values<=count, assigns it to n and prints it
 *------------------------------------------------------------------*/

void producer(int count) 
{
	int32 i;

	//producing values less than 'count', as 'i' starts from '0'
	for(i=0; i<count; i++)
	{
		//assigning the produced value to global variable 'n'
		n=i;
		//printing the produced value; commented as not actually required in the producer
		//printf("produced : %d\n",n);
	}
}

