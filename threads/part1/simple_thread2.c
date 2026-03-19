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

static void *t2_Start(void *arg)
{
	int i = 0;
	for (i = 0; i < 10; i++) {
		printf("%s: In iteration %d\n", __func__, i);
		sleep(2);
	}
	getchar();
	pthread_exit(NULL);

}

static void *t1_Start(void *arg)
{
	pthread_t t2;
	int ret;

	printf("%s: initiating thread t2\n", __func__);

	ret = pthread_create(&t2, NULL, t2_Start, NULL);
	if (ret != 0) {
		perror("Pthread_create \n");
		exit(ret);
	}
	ret = pthread_join(t2, NULL);
	if (ret != 0)
		printf("%s: Join failed :%s\n", __func__, strerror(ret));

	printf("%s:Thread t2 returned\n", __func__);
	pthread_exit(NULL);
}

int main()
{
	pthread_t t1, t2;
	void *res;
	int ret;

	ret = pthread_create(&t1, NULL, t1_Start, NULL);
	if (ret != 0) {
		perror("Pthread_create \n");
		exit(ret);
	}
	printf("%s:Thread 't1' initialised, wait for completion\n", __func__);

	ret = pthread_join(t1, NULL);
	if (ret != 0)
		printf("%s: Join failed :%s\n", __func__, strerror(ret));

	printf("%s:Thread t1 returned\n", __func__);
	return 0;
}
