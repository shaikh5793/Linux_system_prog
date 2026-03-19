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

#include <pthread.h>
#include <stdio.h>
#include <linux/errno.h>
#include <string.h>

void *tStart(void *null)
{
	pthread_attr_t attr;
	int detachstate;
	
	/* access attrib values in force*/
	pthread_getattr_np(pthread_self(), &attr);
	pthread_attr_getdetachstate(&attr, &detachstate);
	if (detachstate == PTHREAD_CREATE_JOINABLE)
		printf("%s: In Joinable Mode (explicit cleanup)\n", __func__);
	else
		printf("%s: In Detached Mode (auto cleanup)\n", __func__);

	sleep(10);

	pthread_getattr_np(pthread_self(), &attr);
	pthread_attr_getdetachstate(&attr, &detachstate);
	if (detachstate == PTHREAD_CREATE_JOINABLE)
		printf("%s: In Joinable Mode (explicit cleanup)\n", __func__);
	else
		printf("%s: In Detached Mode (auto cleanup)\n", __func__);


	pthread_exit(NULL);
}

int main()
{
	int detach_state, rc;
	pthread_t t;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_create(&t, &attr, tStart, NULL);

	sleep(5);
	/* move thread to detached state */
	rc = pthread_detach(t);
	if(rc != 0)
		printf("%s: detach failed: %s\n",__func__, strerror(rc));

	pthread_exit(NULL);
}
