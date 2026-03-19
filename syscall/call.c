# include <stdio.h>

int main()
{
	long res;
	__asm__("movq $338, %rax");
	__asm__("syscall");
	__asm__("movq %rax, -8(%rbp)");

	printf("val returned by syscall %ld\n",res);
	return 0;
}
