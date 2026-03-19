/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 */
# include <stdio.h>
# include <dlfcn.h>
# include <stdlib.h>

int main()
{

	void *handle;
 
	/* step 1: Declare function pointer */
	void (*fnptr)(void);
	printf("%s: start of main\n",__func__);
	getchar();

/* step 2: request link-loader to load specified library and 
           attach it into our address space */

	handle = dlopen("./libtest.so", RTLD_NOW);
	if(handle == NULL){
		printf(" Failed to load library \n");
		exit(2);
	}
/* step 3: lookup for the address of the required function */

	fnptr = dlsym(handle,"lib1_func1");
	if(fnptr == NULL){
		printf(" Failed to find symbol \n");
		dlclose(handle);
		exit(2);

    }
	getchar();

/* step 4: invoke function through  pointer */
	(fnptr)();// indirect call

/* step 5: request link-loader to unlink library */
	dlclose(handle);

	printf("%s: end of main\n",__func__);
	getchar();
	return 0;
}	
