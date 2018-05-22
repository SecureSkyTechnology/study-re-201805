#include <stdio.h>
#include <windows.h>

typedef VOID (WINAPI * PGetLocalTime)(LPSYSTEMTIME);
static PGetLocalTime pGetLocalTimeOriginal = NULL;
static PGetLocalTime pGetLocalTimeKernelbase = NULL;

VOID __stdcall MyGetLocalTime(LPSYSTEMTIME lpSystemTime)
{
    pGetLocalTimeKernelbase(lpSystemTime);
    lpSystemTime->wYear = 2000;
    return;
}

void printGetLocalTime()
{
    SYSTEMTIME st;
    pGetLocalTimeOriginal(&st);
    printf("%04d-%02d-%02d %02d:%02d:%02d.%03d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}

void MakeJMP(BYTE *pAddress, DWORD dwJumpTo, DWORD dwLen) {
    DWORD dwOldProtect, dwBkup, dwRelAddr;
    VirtualProtect(pAddress, dwLen, PAGE_EXECUTE_READWRITE, &dwOldProtect);
    dwRelAddr = (DWORD)(dwJumpTo - (DWORD) pAddress) - 5;
    *pAddress = 0xE9;
    *((DWORD*)(pAddress + 0x1)) = dwRelAddr;
    for (DWORD x = 0x5; x < dwLen; x++) {
        *(pAddress + x) = 0x90;
    }
    VirtualProtect(pAddress, 10, dwOldProtect, &dwBkup);
}

int main(int argc, char *argv[]) {
    HMODULE hKernel32 = LoadLibrary("kernel32.dll");
    pGetLocalTimeOriginal = (PGetLocalTime)GetProcAddress(hKernel32, "GetLocalTime");
    printf("kernel32.dll at %p, GetLocalTime is located at %p\n", hKernel32, pGetLocalTimeOriginal);

    HMODULE hKernelbase = LoadLibrary("kernelbase.dll");
    pGetLocalTimeKernelbase = (PGetLocalTime)GetProcAddress(hKernelbase, "GetLocalTime");
    printf("kernelbase.dll at %p, GetLocalTime is located at %p\n", hKernelbase, pGetLocalTimeKernelbase);

    DWORD dwMyGetLocalTimeAddr = (DWORD)(&MyGetLocalTime);
    printf("&MyGetLocalTime = %08p\n", &MyGetLocalTime);
    printf("dwMyGetLocalTimeAddr = %08x\n", dwMyGetLocalTimeAddr);

    printGetLocalTime();

    MakeJMP(pGetLocalTimeOriginal, dwMyGetLocalTimeAddr, 6);

    printGetLocalTime();

    return 0;
}
