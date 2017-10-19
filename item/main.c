#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i;
    for(i = 0; i < 3; i++)
    {
        if(i ==1)
        {
            continue ;
        }
        printf("print the value of i = %d\n", i);
    }
    return 0;
}
