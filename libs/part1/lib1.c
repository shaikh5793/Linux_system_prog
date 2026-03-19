/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 */
#include <stdio.h>

void lib1_func1(void)
{
	printf("%s: Initializing library resources\n", __func__);
	printf("%s: Resource setup completed\n", __func__);
}

void lib1_func2(void)
{
	printf("%s: Executing operation\n", __func__);
	printf("%s: Operation completed successfully\n", __func__);
}


