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
	int ret;

	ret = 0;
	printf("%s:process registered with pid %d\n", __func__, getpid());

   	ptr1 = dlopen("./libone.so", RTLD_NOW | RTLD_GLOBAL);
        if (!ptr1) {
                printf("%s\n", dlerror());
                goto error;
        }

	ptr = dlopen("./libtwo.so", RTLD_NOW | RTLD_DEEPBIND);
	if (!ptr) {
		printf("%s\n", dlerror());
		goto error;
	}

	fptr = dlsym(ptr, "libtwo");
	if (!fptr) {
		printf("%s\n", dlerror());
		goto error;
	}
	(fptr) ();

	getchar();

	/* detach lib from process address space */
	dlclose(ptr);
	getchar();
	printf("%s:lib detached\n", __func__);
	return 0;

 error:
	exit(1);

}
