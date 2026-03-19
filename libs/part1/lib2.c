/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 */
#include <stdio.h>

void lib2_func1(void)
{
	printf("%s: Loading configuration\n", __func__);
	printf("%s: Config loaded successfully\n", __func__);
}

void lib2_func2(void)
{
	printf("%s: Processing data\n", __func__);
	printf("%s: Data processing completed\n", __func__);
}
