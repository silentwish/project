#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i = 10, a = 0, b = 0, c = 0, d = 0;
    a = i++;
    b = i--;
    c = ++i;
    d = --i;
    printf("a=%d, b==%d, c=%d, d=%d\n",a,b,c,d);
    return 0;
}
