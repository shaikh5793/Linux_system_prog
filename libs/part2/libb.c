/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>

extern void b(void);

void x(void)
{
	printf("%s:routine invoked\n", __func__);
}

void y(void)
{
	printf("%s:routine invoked\n", __func__);
	b();
}
