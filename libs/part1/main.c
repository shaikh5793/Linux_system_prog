/*
 * Copyright (c) 2024 
 * Author: Shaikh 
 * SPDX-License-Identifier: MIT
 */
#include <stdio.h>
#include "lib.h"

int main()
{
	printf("%s: test app\n", __func__);
	lib1_func1();
	lib2_func2();
	getchar();
	return 0;
}
