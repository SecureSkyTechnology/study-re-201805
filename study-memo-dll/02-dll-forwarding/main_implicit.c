#include <stdio.h>

__declspec(dllimport) int funcA1(int a, int b);
__declspec(dllimport) int funcB1(int a, int b);

int main() {
    printf("funcA1(2, 3) = %d\n", funcA1(2, 3));
    printf("funcB1(2, 3) = %d\n", funcB1(2, 3));
    return 0;
}