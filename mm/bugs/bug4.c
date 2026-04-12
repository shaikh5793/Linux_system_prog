#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

static void Buggy(void)
{
        char *arr;
	int n = 10;

        arr = malloc(n);
        if (!arr){
                fprintf(stderr,"malloc arr failed\n");
		exit(1);
	}

        memset(arr, 'a', n);
        arr[n-1]='\0';
        printf("%s\n", arr);

        free(arr);
        strcpy(arr,"uaf"); 

        printf("%s\n", arr);
}



int main()
{
	Buggy();
	return 0;
}
