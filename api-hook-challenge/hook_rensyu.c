#include <stdio.h>
#include <windows.h>

void printCurrentTime() {
    SYSTEMTIME time;
    GetLocalTime(&time);
    printf("current min : %d, sec : %d, milli-sec : %d\n", time.wMinute, time.wSecond, time.wMilliseconds);
}

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

int main(int argc, char *argv[]) {
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

    // byte* kernel32_sleep_code = (byte*)pSleepOriginal;
    // DWORD* jmpToKernelbaseSleep = (DWORD*)(kernel32_sleep_code + 2);
    // dwJmpToOriginal = *jmpToKernelbaseSleep;
    // printf("kernel32.Sleep JMP to %08x\n", *jmpToKernelbaseSleep);
    // printf("kernel32.Sleep JMP to %08x(2)\n", dwJmpToOriginal);

    DWORD dwMySleepAddr = (DWORD)(&MySleep);
    printf("&MySleepAddr = %08p\n", &MySleep);
    printf("dwMySleepAddr = %08x\n", dwMySleepAddr);

    __int32 sleepms = 500;
    printCurrentTime();
    Sleep(sleepms);
    printCurrentTime();

    MakeJMP(pSleepOriginal, dwMySleepAddr, 6);

    // DWORD dwOldProtect, dwBackup;
    // VirtualProtect(pSleepOriginal, 10, PAGE_EXECUTE_READWRITE, &dwOldProtect);

    // *jmpToKernelbaseSleep = dwMySleepAddr;
    // // 74696680 >-FF25 6C0B6F74    JMP DWORD PTR DS:[<&KERNELBASE.Sleep>]         ; KERNELBA.Sleep

    // VirtualProtect(pSleepOriginal, 10, dwOldProtect, &dwBackup);

    printCurrentTime();
    Sleep(sleepms);
    printCurrentTime();

    printCurrentTime();
    pSleepOriginal(sleepms);
    printCurrentTime();

    return 0;
}
