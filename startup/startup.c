#include <stdio.h>
#include <stdlib.h>

int xyz()
{
	printf("%s: programs main(xyz()) routine\n",__func__);
	return 0;
}

void _start(void)
{
	printf("%s: Programs entry routine\n",__func__);
	xyz();
	exit(0);
}

/*
gcc -nostartfiles startup.c -o program :  without main() function use this  command so not call from startup files

*/