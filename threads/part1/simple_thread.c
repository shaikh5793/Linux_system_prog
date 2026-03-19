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

static void *tStart(void *arg)
{
	char *s = (char *)arg;
	printf("%s: recv'd arg string %s %ld %d\n", __func__, s, pthread_self(), getpid());

	getchar();
	return (void *)strlen(s);
}

int main()
{
	pthread_t t;
	void *res;
	int ret;

	ret = pthread_create(&t, NULL, tStart, "Hello world\n");
	if (ret != 0) {
		perror("Pthread_create failed \n");
		exit(ret);
	}

	printf("%s: wait for thread completion\n",__func__);

	printf("%s: pid %d\n",__func__,getpid());

	ret = pthread_join(t, &res);
	if (ret != 0)
		printf("%s: pthread join failed\n", __func__);

	
	printf("%s: recv'd on join %ld\n", __func__, (long)res);
//	getchar();
	
	return 0;
}
