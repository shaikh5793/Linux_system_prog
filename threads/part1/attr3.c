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

#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

void *tStart(void *null)
{
	int policy;
	struct sched_param param;
	cpu_set_t cset;

	pthread_getschedparam(pthread_self(), &policy, &param);
	// 0 - SCHED_OTHERS     1 - SCHED_FIFO          2 - SCHED_RR
	printf("%s: Current Policy : %d Priority : %d\n", __func__, policy,
	       param.sched_priority);

	sleep(5);

	
	CPU_ZERO( &cset );
        CPU_SET( 0, &cset);
        CPU_SET( 1, &cset);

	pthread_setaffinity_np(pthread_self(),sizeof(cset), &cset);


	

	pthread_getschedparam(pthread_self(), &policy, &param);
	// 0 - SCHED_OTHERS     1 - SCHED_FIFO          2 - SCHED_RR
	printf("%s: Current Policy : %d Priority : %d\n", __func__, policy,
	       param.sched_priority);

	getchar();
	pthread_exit(NULL);
}

int main()
{
	int inherit, policy, priority, rc;
	pthread_t t;
	pthread_attr_t attr;
	struct sched_param param;

	pthread_attr_init(&attr);

	/* switch off sched inheritence from parent */
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

	/* Assign Sched policy and priority */
	policy = SCHED_FIFO;
	pthread_attr_setschedpolicy(&attr, policy);

	param.sched_priority = 10;
	pthread_attr_setschedparam(&attr, &param);

	/* create thread with choosen attribs */
	pthread_create(&t, &attr, tStart, NULL);

	/* destroy attribute object */
	pthread_attr_destroy(&attr);

	sleep(2);

	param.sched_priority = 20;
	policy = SCHED_RR;
	pthread_setschedparam(t, policy, &param);

	pthread_exit(NULL);
}
