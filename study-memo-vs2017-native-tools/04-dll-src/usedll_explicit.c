#include <stdio.h>
#include <windows.h>

typedef int (CALLBACK* lp_foo)(int, int);
typedef int (CALLBACK* lp_bar)(int, int);

int main() {
    HINSTANCE hDll;
    lp_foo foo;
    lp_bar bar;

    // DLL名を指定してロード
    hDll = LoadLibrary("foobarbaz");
    if (NULL == hDll) {
        printf("dll load error.\n");
        return 1;
    }

    // "foo()"関数のポインタを取得
    foo = (lp_foo)GetProcAddress(hDll, "foo");
    if (!foo) {
        printf("GetProcAddress error(foo).\n");
        FreeLibrary(hDll);
        return 1;
    }

    // "bar()"関数のポインタを取得
    bar = (lp_foo)GetProcAddress(hDll, "bar");
    if (!bar) {
        printf("GetProcAddress error(bar).\n");
        FreeLibrary(hDll);
        return 1;
    }

    printf("foo(2, 3) = %d\n", foo(2, 3));
    printf("bar(2, 3) = %d\n", bar(2, 3));
    return 0;
}