#include <windows.h>

typedef VOID (* PGetLocalTime)(LPSYSTEMTIME);

VOID __stdcall MyGetLocalTime(LPSYSTEMTIME lpSystemTime)
{
    HMODULE hKernel32 = LoadLibrary("kernel32.dll");
    PGetLocalTime pGetLocalTimeOriginal = (PGetLocalTime)GetProcAddress(hKernel32, "GetLocalTime");
    pGetLocalTimeOriginal(lpSystemTime);
    lpSystemTime->wYear = 2000;
    return;
}
