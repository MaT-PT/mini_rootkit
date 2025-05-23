#ifndef __MINI_ROOTKIT_H__
#define __MINI_ROOTKIT_H__

#include <ntifs.h>
#include <wdm.h>
#include "shared/rootkit_uapi.h"

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

static UNICODE_STRING DEVICE_NAME = RTL_CONSTANT_STRING(L"\\Device\\" DEV_NAME);
static UNICODE_STRING DEVICE_LINK = RTL_CONSTANT_STRING(L"\\??\\" DEV_NAME);

#endif  // __MINI_ROOTKIT_H__
