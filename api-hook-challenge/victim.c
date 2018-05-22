#include <stdio.h>
#include <windows.h>

void printCurrentTime() {
    SYSTEMTIME time;
    GetLocalTime(&time);
    printf("current min : %d, sec : %d, milli-sec : %d\n", time.wMinute, time.wSecond, time.wMilliseconds);
}

int main(int argc, char *argv[]) {
    for (int i = 0; i < 500; i++) {
        printCurrentTime();
        Sleep(1000);
        printf("hi-ho ! : %d\n", i);
        printCurrentTime();
    }
    return 0;
}
