#include <stdio.h>
#include <windows.h>

typedef void (WINAPI* PSleep)(DWORD);
static PSleep pSleepOriginal = NULL;
static PSleep pSleepKernelbase = NULL;

static LPVOID lpSleepOriginalJmp = NULL;
static DWORD dwJmpToOriginal = 0;

__declspec(naked) void WINAPI MySleep() {
    DWORD dwMillis;
    __asm {
        PUSH EBP;
        MOV EBP, ESP;
        MOV EAX, [ESP+8];
        MOV [EBP-4], EAX;
    }
    dwMillis /= 2;
    __asm {
        MOV EAX, [EBP-4];
        MOV [ESP+8], EAX;
        POP EBP;
        PUSH dwJmpToOriginal;
        RET;
    }
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
    VirtualProtect(pSleepOriginal, 10, dwOldProtect, &dwBkup);
}

BOOL WINAPI DllMain(
        HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
    HMODULE hModule;
    DWORD dwPid = GetCurrentProcessId();

    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            printf("payload.dll was attached to 0x%08X\n", dwPid);

            HMODULE hKernel32 = LoadLibrary("kernel32.dll");
            pSleepOriginal = (PSleep)GetProcAddress(hKernel32, "Sleep");
            printf("kernel32.dll at %p, Sleep is located at %p, implicitly linked at %p\n",
                hKernel32, pSleepOriginal, &Sleep);

            HMODULE hKernelbase = LoadLibrary("kernelbase.dll");
            pSleepKernelbase = (PSleep)GetProcAddress(hKernelbase, "Sleep");
            printf("kernelbase.dll at %p, Sleep is located at %p\n",
                hKernelbase, pSleepKernelbase);
            dwJmpToOriginal = (DWORD)pSleepKernelbase;
            printf("dwJmpToOriginal = %08x\n", dwJmpToOriginal);

            DWORD dwMySleepAddr = (DWORD)(&MySleep);
            printf("&MySleepAddr = %08p\n", &MySleep);
            printf("dwMySleepAddr = %08x\n", dwMySleepAddr);

            MakeJMP(pSleepOriginal, dwMySleepAddr, 6);

            break;
        case DLL_PROCESS_DETACH:
            printf("payload.dll was detached from 0x%08X\n", dwPid);
            break;
    }
    return TRUE;
}
