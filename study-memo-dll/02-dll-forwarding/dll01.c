#include <windows.h>

int funcA1(int a, int b)
{
    OutputDebugString("funcA1() start\n");
    return a + b;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved)
{
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        OutputDebugString("dll01, DllMain() : DLL_PROCESS_ATTACH\n");
        break;
    case DLL_THREAD_ATTACH:
        OutputDebugString("dll01, DllMain() : DLL_THREAD_ATTACH\n");
        break;
    case DLL_THREAD_DETACH:
        OutputDebugString("dll01, DllMain() : DLL_THREAD_DETACH\n");
        break;
    case DLL_PROCESS_DETACH:
        OutputDebugString("dll01, DllMain() : DLL_PROCESS_DETACH\n");
        break;
    default:
        break;
    }
    return TRUE;
}