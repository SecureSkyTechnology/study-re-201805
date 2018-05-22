#include <stdio.h>
#include <windows.h>

void printGetLocalTime()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    printf("%04d-%02d-%02d %02d:%02d:%02d.%03d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}

int main(int argc, char *argv[]) {
    printGetLocalTime();
    LoadLibrary("hook.dll");
    printGetLocalTime();
    return 0;
}
