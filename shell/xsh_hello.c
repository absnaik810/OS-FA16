/* Program to display Hello World */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

shellcmd xsh_hello(int nargs, char *args[]) {

	if(nargs==2)
		printf("Hello %s\n", args[1]);
	else
		if(nargs<2)
			printf("Arguments less than 2 entered\n");
		else
			printf("Arguments greater than 2 entered\n");

	return 0;
}
