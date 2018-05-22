#include <windows.h>

int __declspec(dllexport) funcB2(int a, int b)
{
    OutputDebugString("funcB2() start\n");
    return a * b;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved)
{
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        OutputDebugString("dll02, DllMain() : DLL_PROCESS_ATTACH\n");
        break;
    case DLL_THREAD_ATTACH:
        OutputDebugString("dll02, DllMain() : DLL_THREAD_ATTACH\n");
        break;
    case DLL_THREAD_DETACH:
        OutputDebugString("dll02, DllMain() : DLL_THREAD_DETACH\n");
        break;
    case DLL_PROCESS_DETACH:
        OutputDebugString("dll02, DllMain() : DLL_PROCESS_DETACH\n");
        break;
    default:
        break;
    }
    return TRUE;
}