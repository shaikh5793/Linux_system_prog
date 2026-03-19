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

int *ret;
static void *joinableThread(void *x)
{
	int i = 0;
	ret = (int *)x;

	unsigned int tid;
	tid = (unsigned int)pthread_self();
	printf("%s:Joinable Thread id %u\n", __func__, tid);
	for (i = 0; i < 10; i++) {
		printf("%s: In iteration %d\n", __func__, i);
		sleep(2);
	}
	return (void *)ret;
}

static void *detachedThread(void *x)
{
	int i = 0;
	unsigned int tid;
	tid = (unsigned int)pthread_self();

	printf("%s:detached Thread id %u\n", __func__, tid);

	for (i = 0; i < 10; i++) {
		printf("%s: In iteration %d\n", __func__, i);
		sleep(5);
	}
	pthread_exit(NULL);
}

int main()
{
	pthread_t t, t1;
	pthread_attr_t attr;
	int ret;
	void *res;

	/* init defaults */
	ret = pthread_attr_init(&attr);
	if (ret != 0)
		perror("pthread_attr_init");

	ret = pthread_create(&t, NULL, joinableThread, (void *)10);
	if (ret != 0) {
		perror("pthread_create");
		exit(ret);
	}

	/* assign detached state */
	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (ret != 0)
		perror("pthread_attr_getdetachstate");

	ret = pthread_create(&t1, &attr, detachedThread, NULL);
	if (ret != 0) {
		perror("pthread_create");
		exit(ret);
	}

	ret = pthread_join(t, &res);
	if (ret != 0)
		printf("%s: pthread_join failed on Joinable thread(t): %s\n",
		       __func__, strerror(ret));

	printf("%s: Joinable Thread 't' returned %ld\n", __func__, (long)res);

	printf("%s: Attempt to join with 't1'(detached thread)\n", __func__);
	ret = pthread_join(t1, &res);
	if (ret != 0)
		printf("%s: pthread_join failed on detached thread(t1): %s\n",
		       __func__, strerror(ret));

	pthread_exit(NULL);
}
