#include <stdio.h>
#include <stdlib.h>

int main()
{
    int a = 5, b = 10;
    while(b >  a)
    {
        printf("while looping a = %d, b = %d\n",a, b);
        if(a == 8)
        break;
        a++;
    }
    printf("while loop over\n");

    return 0;
}
