#include <stdio.h>
#include <windows.h>

void printCurrentTime() {
    SYSTEMTIME time;
    GetLocalTime(&time);
    printf("current min : %d, sec : %d, milli-sec : %d\n", time.wMinute, time.wSecond, time.wMilliseconds);
}

int main(int argc, char *argv[]) {
    __int32 sleepms = 500;
    printCurrentTime();
    Sleep(sleepms);
    printCurrentTime();
    printCurrentTime();
    Sleep(sleepms);
    printCurrentTime();

    LoadLibrary("hook.dll");

    printCurrentTime();
    Sleep(sleepms);
    printCurrentTime();
    printCurrentTime();
    Sleep(sleepms);
    printCurrentTime();

    return 0;
}
