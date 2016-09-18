/* Program to display Hello World */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*---------------------------------------------------------------------------------
 *	xsh_hello - prints hello <name>; also provided with a --help argument
 ---------------------------------------------------------------------------------*/

shellcmd xsh_hello(int nargs, char *args[]) {

	if(nargs==2)
	{
		if(strcmp(args[1],"--help")==0)
			printf("This is just a basic hello command.  Just type in 'hello' followed by your name; and the program will output Hello <name>\n");
		else
			printf("Hello %s\n", args[1]);
	}
	else
		if(nargs<2)
			printf("Less arguments entered\n");
		else
			printf("Many arguments entered\n");

	return 0;
}
