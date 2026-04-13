#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

static void Buggy(void)
{
	 char *p = malloc(8);
         if (!p){
                printf("malloc failed\n");
	 	exit(1);
	 }

        p--;
        strncpy(p, "abcd5678", 8); 
	printf("%s\n", p);
        free(++p);


}

int main()
{
	Buggy();
	return 0;
}
