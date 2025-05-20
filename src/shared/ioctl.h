#ifndef __IOCTL_H__
#define __IOCTL_H__

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

#endif  // __IOCTL_H__
