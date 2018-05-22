#include <windows.h>
 
int __declspec(dllexport) foo(int a, int b)
{
    OutputDebugString("foo() start\n");
    return a + b;
}
 
int __declspec(dllexport) bar(int a, int b)
{
    OutputDebugString("bar() start\n");
    return a * b;
}
 
#define MAKE_FUNCX(N) int func##N(int a, int b) { return a + b + N; }
 
MAKE_FUNCX(1)
MAKE_FUNCX(2)
MAKE_FUNCX(3)
MAKE_FUNCX(4)
MAKE_FUNCX(5)
MAKE_FUNCX(6)
MAKE_FUNCX(7)
 
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved)
{
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        OutputDebugString("DllMain() : DLL_PROCESS_ATTACH\n");
        break;
    case DLL_THREAD_ATTACH:
        OutputDebugString("DllMain() : DLL_THREAD_ATTACH\n");
        break;
    case DLL_THREAD_DETACH:
        OutputDebugString("DllMain() : DLL_THREAD_DETACH\n");
        break;
    case DLL_PROCESS_DETACH:
        OutputDebugString("DllMain() : DLL_PROCESS_DETACH\n");
        break;
    default:
        break;
    }
    return TRUE;
}
