#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>

// {{{ _print_last_error()

static void
_print_last_error(DWORD err)
{
    LPTSTR lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
            | FORMAT_MESSAGE_FROM_SYSTEM 
            | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, err,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf, 0, NULL);
    fprintf(stderr, "%s\n", lpMsgBuf);
    LocalFree(lpMsgBuf);
}

// }}}
// {{{ find_proc_id()

DWORD find_proc_id(const char *exe_filename)
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    DWORD dwProcId = 0;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hProcessSnap) {
        return 0;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32)) {
        return 0;
    }

    do {
        if (!strcmp(pe32.szExeFile, exe_filename)) {
            dwProcId = pe32.th32ProcessID;
            break;
       }
    } while (Process32Next(hProcessSnap, &pe32));
    CloseHandle(hProcessSnap);
    return dwProcId;
}

// }}}

int main(int argc, char *argv[])
{
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwDestProcId;
    DWORD dwThreadId;
    DWORD dwThreadExitCode;
    FARPROC lpvLoadLibraryA = NULL;
    LPVOID lpvDllName = NULL;
    char *dllname = "hook.dll";
    char *targetname = "victim.exe";
    SIZE_T szbuf;

    // find target process and get its process id.
    dwDestProcId = find_proc_id(targetname);
    if (0 == dwDestProcId) {
        fprintf(stderr, "%s process was not found.\n", targetname);
        return 1;
    }
    printf("%s was found, process id is 0x%08X\n", targetname, dwDestProcId);

    // get process handle for target process
    hProcess = OpenProcess(
            PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
            PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
            FALSE, dwDestProcId);
    if (NULL == hProcess) {
        _print_last_error(GetLastError());
        fprintf(stderr, "OpenProcess() failed\n");
        return 2;
    }

    // write dll filename to target process memory
    lpvDllName = VirtualAllocEx(hProcess, NULL, 
            strlen(dllname) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (NULL == lpvDllName) {
        _print_last_error(GetLastError());
        fprintf(stderr, "VirtualAllocEx() failed\n");
        return 3;
    }
    if (!WriteProcessMemory(
                hProcess,
                lpvDllName,
                (LPVOID)dllname,
                strlen(dllname) + 1,
                &szbuf)) {
        _print_last_error(GetLastError());
        fprintf(stderr, "WriteProcessMemory() failed\n");
        return 4;
    }

    // get LoadLibraryA address
    lpvLoadLibraryA = GetProcAddress(
            GetModuleHandle("kernel32.dll"),
            "LoadLibraryA");

    // kick LoadLibraryA in target process
    hThread = CreateRemoteThread(
            hProcess,
            NULL, // LPSECURITY_ATTRIBUTES
            0, // SIZE_T dwStackSize (use system default)
            (LPTHREAD_START_ROUTINE)lpvLoadLibraryA,
            (LPVOID)lpvDllName, // LPVOID lpParameter
            0, // DWORD dwCreationFlags (use default flag)
            &dwThreadId);
    if (NULL == hThread) {
        _print_last_error(GetLastError());
        fprintf(stderr, "CreateRemoteThread() failed\n");
        return 5;
    }
    printf("thread id = 0x%08X\n", dwThreadId);

    // wait until LoadLibraryA thread.
    WaitForSingleObject(hThread, INFINITE);
    GetExitCodeThread(hThread, &dwThreadExitCode);

    CloseHandle(hThread);

    if (!VirtualFreeEx(hProcess, lpvDllName, 0, MEM_RELEASE)) {
        _print_last_error(GetLastError());
        fprintf(stderr, "VirtualFreeEx() failed\n");
    }

    CloseHandle(hProcess);

    return 0;
}
