#include <windows.h>
#include <stdio.h>

int __declspec(dllexport) foo(int a, int b)
{
    return a + b;
}

int __declspec(dllexport) bar(int a, int b)
{
    return a * b;
}

#define SZ_MODNAME 512
#define SZ_ATTACHMSG 1024

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved)
{
    char modname[SZ_MODNAME];
    char attachmsg[SZ_ATTACHMSG];
    GetModuleFileName(hInstance, modname, SZ_MODNAME);

    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        snprintf(attachmsg, SZ_ATTACHMSG, "DllMain() : DLL_PROCESS_ATTACH at %p (%s)\n", hInstance, attachmsg);
        OutputDebugString(attachmsg);
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
