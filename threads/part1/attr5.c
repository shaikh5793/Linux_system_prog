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

#define _GNU_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>


void *tStart(void *arg)
{
	void *getStkAddr;
	size_t getStkSize;

	pthread_attr_t attr;
	pthread_getattr_np(pthread_self(), &attr);
	pthread_attr_getstack(&attr, &getStkAddr, &getStkSize);
	printf("%s:Stack starts @ add :%p of size :%d\n", __func__,
	       getStkAddr, (int)getStkSize);

	pthread_exit(NULL);
}

int main()
{
	pthread_t t;
	pthread_attr_t attr;

	size_t stacksize = 1024 * 16;
	void *stackaddr;
	int align = getpagesize();

	/* allocate memory with page alignment */
	posix_memalign(&stackaddr, align, stacksize);

	printf("%s: memory starts @ add :%p of size :%d\n", __func__, stackaddr,
	       (int)stacksize);

	pthread_attr_init(&attr);
	pthread_attr_setstack(&attr, stackaddr, stacksize);
	pthread_create(&t, &attr, tStart, NULL);
	pthread_exit(NULL);
}
