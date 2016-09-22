/* Code pertaining to the prodcons command */

#include<xinu.h>
#include<prodcons.h>

int n;		//Global variable n

/*------------------------------------------------------------------------
 * xsh_prodcons - creates two processes, the producer and the consumer
 *----------------------------------------------------------------------*/

shellcmd xsh_prodcons(int nargs, char *args[])
{
	int count;
	//if there is only one argument, then it is the file name; so set default count=2000;
	if(nargs==1)
		count=2000;
	else
		if(nargs==2)
		{
			count=atoi(args[1]);
			if(count==0)		//atoi() failed; which means input was not an integer; so default count=2000;
				count=2000;
		}
		else
		{
			printf("Too many arguments.  Exiting...\n");
			return 0;
		}

	resume(create(producer, 1024, 20, "producer", 1, count));
	resume(create(consumer, 1024, 20, "consumer", 1, count));

	return 0;
}
