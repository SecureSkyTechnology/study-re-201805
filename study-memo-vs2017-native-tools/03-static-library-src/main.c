#include <stdio.h>

int foo(int a, int b);
int bar(int a, int b);

int main() {
    printf("foo(2, 3) = %d\n", foo(2, 3));
    printf("bar(2, 3) = %d\n", bar(2, 3));
    return 0;
}