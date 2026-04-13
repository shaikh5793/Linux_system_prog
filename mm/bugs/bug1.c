#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>


static void Buggy(void)
{
        char *arr;

        arr = (char *)malloc(5);
        if (!arr){
		printf("Allocation failed\n");
		exit(1);
	}

        memset(arr, 'a', 5);
        
	arr[5] = 'w'; arr[6] = 'r'; arr[7] = 'o';
        arr[8] = 'o'; arr[9] = 'b';

	
        printf("arr = %s\n", arr);

        printf("*(arr+100)=%d\n", *(arr + 100));
        printf("*(arr+10000)=%d\n", *(arr + 10000));

        free(arr);
}

int main()
{
	Buggy();
	return 0;
}
/*
use Valgrind tools or fsanitized library 
*/
