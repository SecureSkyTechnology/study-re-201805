#include <windows.h>

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