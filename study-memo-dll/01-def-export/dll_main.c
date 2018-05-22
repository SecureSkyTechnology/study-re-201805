#include <windows.h>
#include <stdio.h>

typedef int (CALLBACK* DLLFUNC)(int, int);

__declspec(dllimport) int foo(int a, int b);
__declspec(dllimport) int bar(int a, int b);
__declspec(dllimport) int func1(int a, int b);
__declspec(dllimport) int func2(int a, int b);
__declspec(dllimport) int func3(int a, int b);
/* __declspec(dllimport) int func4(int a, int b); */
/* __declspec(dllimport) int func5(int a, int b); */
__declspec(dllimport) int funcX(int a, int b); /* func6 */
/* __declspec(dllimport) int funcY(int a, int b); */ /* func7 */

int main() {
    HINSTANCE hDll;
    DLLFUNC func4 = NULL;
    DLLFUNC func5 = NULL;
    DLLFUNC funcY = NULL;

    hDll = LoadLibrary("dll_lib");
    if (NULL == hDll) {
        printf("dll load error.\n");
        return 1;
    }
    printf("dll module handle = %p\n", hDll);

    /* func4 */
    func4 = (DLLFUNC)GetProcAddress(hDll, "func4");
    if (!func4) {
        printf("GetProcAddress error(func4).\n");
        FreeLibrary(hDll);
        return 2;
    }

    /* func5 */
    func5 = (DLLFUNC)GetProcAddress(hDll, MAKEINTRESOURCEA(8));
    if (!func5) {
        printf("GetProcAddress error(func5).\n");
        FreeLibrary(hDll);
        return 3;
    }

    /* funcY */
    funcY = (DLLFUNC)GetProcAddress(hDll, "funcY");
    if (!funcY) {
        printf("GetProcAddress error(funcY).\n");
        FreeLibrary(hDll);
        return 4;
    }

    printf("foo(2, 3) = %d\n", foo(2, 3));
    printf("bar(2, 3) = %d\n", bar(2, 3));
    printf("func1(2, 3) = %d\n", func1(2, 3));
    printf("func2(2, 3) = %d\n", func2(2, 3));
    printf("func3(2, 3) = %d\n", func3(2, 3));
    printf("func4(2, 3) = %d\n", func4(2, 3));
    printf("func5(2, 3) = %d\n", func5(2, 3));
    printf("funcX(2, 3) = %d\n", funcX(2, 3));
    printf("funcY(2, 3) = %d\n", funcY(2, 3));
    FreeLibrary(hDll);
    return 0;
}