#include <stdio.h>
#include <stdlib.h>

int main()
{
    int a = 10,  b= 10;
    do
    {
        printf("while looping a = %d, b = %d\n", a, b);
    }while(b > a);
    printf("while loop over\n");
    return 0;
}
