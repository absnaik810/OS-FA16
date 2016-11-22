#include<xinu.h>
#include<future.h>

uint future_prod(future* fut);
uint future_cons(future* fut);

shellcmd xsh_prodcons(int nargs, char *args[])
{
	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use of this command: %s\n", args[0]);
		printf("Please type 'prodcons -f' to execute this program\n");
		return 0;
	}
	
	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Please use '%s --help' for more info\n", args[0]);
		return 1;
	}

	if(nargs==2 && strcmp(args[1],"-f")==0){
	  future *f_exclusive, *f_shared, *f_queue;
 
          //f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
	  f_shared = future_alloc(FUTURE_SHARED);
  	  //f_queue = future_alloc(FUTURE_QUEUE);
 
	  // Test FUTURE_EXCLUSIVE
	  //resume( create(future_cons, 1024, 20, "fcons1", 1, f_exclusive) );
 	  //resume( create(future_prod, 1024, 20, "fprod1", 1, f_exclusive) );

	  // Test FUTURE_SHARED
	  resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) );
 	  resume( create(future_cons, 1024, 20, "fcons3", 1, f_shared) );
 	  resume( create(future_cons, 1024, 20, "fcons4", 1, f_shared) ); 
	  resume( create(future_cons, 1024, 20, "fcons5", 1, f_shared) );
	  resume( create(future_prod, 1024, 20, "fprod2", 1, f_shared) );

 	  // Test FUTURE_QUEUE
  	  //resume( create(future_cons, 1024, 20, "fcons6", 1, f_queue) );
	  //resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
 	  //resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
	  //resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
	  //resume( create(future_prod, 1024, 20, "fprod3", 1, f_queue) );
	  //resume( create(future_prod, 1024, 20, "fprod4", 1, f_queue) );
 	  //resume( create(future_prod, 1024, 20, "fprod5", 1, f_queue) );
 	  //resume( create(future_prod, 1024, 20, "fprod6", 1, f_queue) );
	}
}