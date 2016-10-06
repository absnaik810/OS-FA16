/* Code pertaining to the prodcons command */

#include<xinu.h>
#include<prodcons.h>
#include<future.h>

uint future_prod(future* fut);
uint future_cons(future* fut);
/*-------------------------------------------------------------------------------------------------------------------------
 * xsh_prodcons - creates two processes for testing the futures concept
 *-------------------------------------------------------------------------------------------------------------------------*/

shellcmd xsh_prodcons(int nargs, char *args[])
{
	if(nargs==2 && strcmp(args[1],"-f")==0){

	future *f1, *f2, *f3;
	f1=future_alloc(FUTURE_EXCLUSIVE);	
	f2=future_alloc(FUTURE_EXCLUSIVE);
	f3=future_alloc(FUTURE_EXCLUSIVE);

	resume(create(future_cons, 1024, 20, "fcons1", 1, f1));
	resume(create(future_prod, 1024, 20, "fprod1", 1, f1));	
	resume(create(future_cons, 1024, 20, "fcons2", 1, f2));
	resume(create(future_prod, 1024, 20, "fprod2", 1, f2));	
	resume(create(future_cons, 1024, 20, "fcons3", 1, f3));
	resume(create(future_prod, 1024, 20, "fprod3", 1, f3));	
	}

	return 0;
}

