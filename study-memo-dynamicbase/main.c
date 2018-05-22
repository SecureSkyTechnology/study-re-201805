#include <stdio.h>
#include <windows.h>

typedef int (CALLBACK* lp_foo)(int, int);
typedef int (CALLBACK* lp_bar)(int, int);

int main() {
    HINSTANCE hDll_aslr;
    HINSTANCE hDll_noaslr;
    lp_foo foo;
    lp_bar bar;
    char *dllfn_aslr = "dll_lib_aslr.dll";
    char *dllfn_noaslr = "dll_lib_noaslr.dll";

    HMODULE me = GetModuleHandle(NULL);
    printf("I'm loaded at %p\n", me);

    hDll_aslr = LoadLibrary(dllfn_aslr);
    if (NULL == hDll_aslr) {
        printf("%s load error.\n", dllfn_aslr);
        return 1;
    }
    printf("%20s loaded at %p\n", dllfn_aslr, hDll_aslr);

    hDll_noaslr = LoadLibrary(dllfn_noaslr);
    if (NULL == hDll_noaslr) {
        printf("%s load error.\n", dllfn_noaslr);
        return 1;
    }
    printf("%20s loaded at %p\n", dllfn_noaslr, hDll_noaslr);

    foo = (lp_foo)GetProcAddress(hDll_aslr, "foo");
    if (!foo) {
        printf("GetProcAddress error(foo).\n");
        FreeLibrary(hDll_aslr);
        return 1;
    }

    bar = (lp_foo)GetProcAddress(hDll_noaslr, "bar");
    if (!bar) {
        printf("GetProcAddress error(bar).\n");
        FreeLibrary(hDll_noaslr);
        return 1;
    }

    printf("foo(2, 3) in %s = %d\n", dllfn_aslr, foo(2, 3));
    printf("bar(2, 3) in %s = %d\n", dllfn_noaslr, bar(2, 3));
    return 0;
}