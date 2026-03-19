/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * ******************************************************************************
 * This program is part of the source code provided with
 * "Linux Systems Programming Essentials" Course offered by TECHVEDA(c) 2020 - 2022
 * Course page : https://www.techveda.org/linuxsysprg
 * Trainer:  Raghu Bharadwaj
 * ******************************************************************************
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define NUM_CHUNKS 150

int main(int argc, char *argv[])
{
	void *p[NUM_CHUNKS], *p1;
	int size, count = 0;
	if (argc != 2) {
		fprintf(stderr, "usage :./a.out <num bytes to allocate> \n");
		exit(1);
	}

	size = atoi(argv[1]);

	for (count = 0; count <= NUM_CHUNKS; count++) {
		p[count] = malloc(size * 1024);
		if (!p[count]) {
			fprintf(stderr, "malloc failed\n");
			exit(1);
		}
		printf("allocated @ %p\n", p[count]);
		malloc_stats();
		getchar();
	}

	for (count = 0; count <= 70; count++) {
		free(p[count]);
		malloc_stats();
		getchar();
	}

	printf("Attempting large(>135k)  allocation\n");
	getchar();
	p1 = malloc(140 * 1024);
	if (!p1) {
		fprintf(stderr, "malloc failed\n");
		exit(1);
	}

	malloc_stats();
	getchar();

	return 0;
}
