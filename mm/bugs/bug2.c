#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>


static void Buggy(void)
{
        char arr[5], tmp[8];
        memset(arr, 'a', 5);
        memset(tmp, 't', 8);
        tmp[7] = '\0';

        printf("arr = %s\n", arr);    

  	for (int i = 0; i <= 10; i++) {
                arr[i] = 100;  
        }
	
        printf("arr = %s\n", arr);    

}

int main()
{
	Buggy();
	return 0;
}
