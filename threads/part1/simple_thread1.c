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
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

typedef struct {
	int a;
	int b;
} ABC;

static void *tStart(void *arg)
{
	ABC *p = (ABC *) arg;
	printf("\n%s: In start routine of thread with id %u\n", __func__,
	       (unsigned int)pthread_self());
	printf("%s: recv'd paramenter instance with state %d & %d\n", __func__,
	       p->a, p->b);
	getchar();

	pthread_exit(NULL);
}

int main()
{
	pthread_t t1, t2;
	void *res;
	int ret;

	ABC obj;
	obj.a = 10;
	obj.b = 20;

	ret = pthread_create(&t1, NULL, tStart, &obj);
	if (ret != 0) {
		perror("Pthread_create \n");
		exit(ret);
	}

	ret = pthread_create(&t2, NULL, tStart, &obj);
	if (ret != 0) {
		perror("Pthread_create \n");
		exit(ret);
	}

	printf("%s:Threads initialised, wait for completion\n", __func__);
	pthread_exit(NULL);

	printf("end of main\n");
	return 0;
}
