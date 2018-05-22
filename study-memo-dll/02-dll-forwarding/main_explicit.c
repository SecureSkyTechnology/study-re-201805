#include <windows.h>
#include <stdio.h>

typedef int (CALLBACK* DLLFUNC)(int, int);

int main() {
    HINSTANCE hDll;
    DLLFUNC funcA1 = NULL;
    DLLFUNC funcB1 = NULL;

    hDll = LoadLibrary("dll01");
    if (NULL == hDll) {
        printf("dll load error.\n");
        return 1;
    }

    funcA1 = (DLLFUNC)GetProcAddress(hDll, "funcA1");
    if (!funcA1) {
        printf("GetProcAddress error(funcA1).\n");
        FreeLibrary(hDll);
        return 2;
    }

    funcB1 = (DLLFUNC)GetProcAddress(hDll, "funcB1");
    if (!funcB1) {
        printf("GetProcAddress error(funcB1).\n");
        FreeLibrary(hDll);
        return 3;
    }

    printf("funcA1(2, 3) = %d\n", funcA1(2, 3));
    printf("funcB1(2, 3) = %d\n", funcB1(2, 3));
    FreeLibrary(hDll);
    return 0;
}