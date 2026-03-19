/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 */
#include <stdio.h>

void __attribute__((constructor)) init_routine(void);
void __attribute__((destructor)) fini_routine(void);

void test(void)
{
	printf("%s: test() invoked\n", __func__);
	printf("%s: file %s\n", __func__, __FILE__);
}

void init_routine(void)
{
	printf("%s: in library init routine\n", __func__);
}

void fini_routine(void)
{
	printf("%s: in library fini routine\n", __func__);
}

