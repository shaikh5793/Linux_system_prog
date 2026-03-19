/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 */

# include <stdio.h>

extern void libone(void);

void common()
{
	printf("libtwo common\n");
}
void libtwo()
{
	libone();
	common();
}
