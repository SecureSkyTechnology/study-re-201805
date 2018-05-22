#include <stdio.h>

extern int foo(int a, int b, int c, int d);
extern int bar(int a, int b, int c, int d);

int main(int argc, char *argv[])
{
    int r;
    r = foo(4, 3, 2, 1);
    printf("foo() = %d\n", r);
    r = bar(4, 3, 2, 1);
    printf("bar() = %d\n", r);
    return 0;
}