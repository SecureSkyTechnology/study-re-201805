#include <windows.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Hello, World!\n");
    return 0;
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    MessageBoxA(
        NULL,
        "Hello, Windows GUI Applications!",
        "Hello, World!",
        MB_OK);

    return 0;
}