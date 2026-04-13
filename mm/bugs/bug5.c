#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

static void Buggy(void)
{
  	void *p;
        int i = 0;
	int a;

	printf("%d\n", a);

        while(i < 10){
        	p = (void *) malloc(100);
		if(!p){
			fprintf(stderr, "malloc failed\n");
			exit(1);
		}
		printf("allocated mem @ %p\n",p);
		i++;
		getchar();

        }
	free(p);

}


int main()
{
	Buggy();
	return 0;
}
