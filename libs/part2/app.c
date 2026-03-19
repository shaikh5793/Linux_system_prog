/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	void *ptr,*ptr1;
	void (*fptr) (void);
	int ret = 0;

	printf("%s:process registered with pid %d\n", __func__, getpid());

	ptr1 = dlopen("./liba.so", RTLD_NOW | RTLD_GLOBAL);
	if (!ptr1) {
		printf("%s\n", dlerror());
		goto error;
    }

	ptr = dlopen("./libb.so", RTLD_NOW);
	if (!ptr) {
		printf("%s\n", dlerror());
		goto error;
	}

	fptr = dlsym(ptr, "x");
	if (!fptr) {
		printf("%s\n", dlerror());
		goto error;
	}
	(fptr) ();

	/* find address of a symbol */
	fptr = dlsym(ptr, "y");
	if (!fptr) {
		printf("%s\n", dlerror());
		goto error;
	}
	(fptr) ();		/* indirect call */

	/* detach lib from process address space */
    getchar();
	dlclose(ptr);
    
	printf("%s:lib detached\n", __func__);
	return 0;

 error:
	exit(1);

}
