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
#include <linux/errno.h>
#include <pthread.h>

void *t1_start(void *null)
{
	int policy, i;
	struct sched_param param;
	for (i = 0; i < 100; i++) {
		pthread_getschedparam(pthread_self(), &policy, &param);
		// 0 - SCHED_OTHERS     1 - SCHED_FIFO          2 - SCHED_RR
		printf("%u:Thread loop count :%d Policy : %d Priority : %d\n",
		       (unsigned int)pthread_self(), i, policy,
		       param.sched_priority);
	}

}
void *t_start(void *null)
{
	int policy, i;
	struct sched_param param;
	for (i = 0; i < 100; i++) {
		pthread_getschedparam(pthread_self(), &policy, &param);
		// 0 - SCHED_OTHERS     1 - SCHED_FIFO          2 - SCHED_RR
		printf("%u:Thread loop count :%d Policy : %d Priority : %d\n",
		       (unsigned int)pthread_self(), i, policy,
		       param.sched_priority);
		sleep(1);
	}

}

int main()
{
	int inherit, policy, priority, rc;
	pthread_t t, t1;
	pthread_attr_t attr;
	struct sched_param param;

	pthread_attr_init(&attr);

	/* switch off sched inheritence from parent */
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

	/* Now assign Sched policy and priority */
	policy = SCHED_FIFO;
	pthread_attr_setschedpolicy(&attr, policy);

	param.sched_priority = 5;
	pthread_attr_setschedparam(&attr, &param);

	/* create thread with choosen attribs */
	pthread_create(&t, &attr, t_start, NULL);

	policy = SCHED_RR;
	pthread_attr_setschedpolicy(&attr, policy);

	param.sched_priority = 10;
	pthread_attr_setschedparam(&attr, &param);

	pthread_create(&t1, &attr, t1_start, NULL);

	/* destroy attribute object */
	pthread_attr_destroy(&attr);

	pthread_exit(NULL);

}
