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

#include <unistd.h>
#include <stdio.h>

void *curr_brk, *old_brk, *new_brk;

int main()
{
	int ret;
	int *p;

	printf("[%d]:retrieve heap start\n",getpid());
	getchar();	
	/* grab current program break address */
	curr_brk = sbrk(0);
	old_brk = curr_brk;

	printf("\n %p \n", curr_brk);
//	getchar();

	/*increase program brk by 100 bytes*/
	brk(curr_brk + 100);

	/* verify change */
	new_brk = sbrk(0);
	printf("\n %p \n", new_brk);
	old_brk = new_brk;
//	getchar();

	/*increase program brk by 200 bytes*/
	brk(new_brk + 200);
	getchar();

	/* verify change */
	new_brk = sbrk(0);
	printf("\n %p \n", new_brk);
	getchar();

	/* restore program break to previous address */
	brk(old_brk);
	curr_brk= sbrk(0);
	printf("\n %p \n",curr_brk);
	getchar();
	return 0;
}
