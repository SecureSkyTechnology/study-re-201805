#include <stdio.h>
 
void div0(long p1, long p2, long p3)
{
    long x = p1 + p2 + p3;
    long y = 0;
    y = x / p2;
    printf("ret = %ld\n", y);
}

int main(int argc, char *argv[])
{
    long a = 2;
    long b = 0;
    div0(a, b, 3);

    printf("Hello, World!\n");
    return 0;
}