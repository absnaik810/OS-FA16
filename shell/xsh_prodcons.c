#include <xinu.h>
#include <string.h>
#include <future.h>
#include <stdio.h>
#include <prodcons.h>

int n;                 
sid32 produced,consumed;

uint future_prod(future* fut);
uint future_cons(future* fut);
shellcmd xsh_prodcons(int nargs, char *args[]) {
	future *f1, *f2, *f3;
 
	f1 = future_alloc(FUTURE_EXCLUSIVE);
	f2 = future_alloc(FUTURE_EXCLUSIVE);
	f3 = future_alloc(FUTURE_EXCLUSIVE);
	  
	if(nargs>2) {
		fprintf(stderr, "%s: Many arguments entered.  Please enter correct number of arguments. \n", args[0]);
		return 0;
	}
	else if(nargs==2) {
		if (strncmp(args[1], "--help", 6) == 0) {
			printf("%s: Please enter Prodcons followed by -f \n", args[0]);
			return 0;
		} 
		else {
			if(strcmp(args[1],"-f")==0){
				resume(create(future_cons, 1024, 20, "fcons1", 1, f1));
				resume(create(future_prod, 1024, 20, "fprod1", 1, f1));	
				resume(create(future_cons, 1024, 20, "fcons2", 1, f2));
				resume(create(future_prod, 1024, 20, "fprod2", 1, f2));	
				resume(create(future_cons, 1024, 20, "fcons3", 1, f3));
				resume(create(future_prod, 1024, 20, "fprod3", 1, f3));	
			}
			else
			{
				fprintf(stderr, "%s: Please enter Prodcons followed by -f  \n", args[0]);
			}
		}
	}
	else
	{
		fprintf(stderr,"Less number of arguments entered, so please enter prodcons followed by -f", args[0]);
	}
	return 0;
}