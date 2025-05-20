#ifndef _COMPANION_H_
#define _COMPANION_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <string.h>

#define S_ROOT_PID "root_pid"
#define S_HIDE_PID "hide_pid"
#define S_SHOW_PID "show_pid"
#define S_START_PROCESS "start_process"

typedef enum _FUNCTION {
    FN_INVALID = -1,
    FN_ROOT_PID = 0,
    FN_HIDE_PID = 1,
    FN_SHOW_PID = 2,
    FN_START_PROCESS = 3,
} FUNCTION;

#define CHECK_ARG(_sArg, _sValue) (!strncmp((_sArg), (_sValue), (sizeof(_sValue) - 1)))

static inline VOID PrintError(IN CONST LPCSTR lpMsg) {
    // Get the latest error ID
    CONST DWORD dwErrId = GetLastError();
    printf("[ERR: %d] %s: ", dwErrId, lpMsg);

    // Pring the error message based on the response
    if (dwErrId) {
        LPSTR lpMsgBuf;
        CONST DWORD dwRes = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, dwErrId, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&lpMsgBuf, 0, NULL);
        if (dwRes) {
            printf("%s\n", lpMsgBuf);
            LocalFree(lpMsgBuf);
        } else {
            printf("Unknown error\n");
        }
    } else {
        printf("Something went wrong\n");
    }
}

static inline VOID PrintIoctlError() { PrintError("Failed to send IOCTL"); }

DWORD StartProcess(IN LPSTR lpCmd, IN DWORD dwCreationFlags, IN BOOL bWait);

#endif  // _COMPANION_H_
