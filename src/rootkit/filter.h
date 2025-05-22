#ifndef __FILTER_H__
#define __FILTER_H__

#include <fltKernel.h>
#include <ntifs.h>
#include <wdm.h>
// Include minwindef.h after ntifs.h and wdm.h to avoid redefinition errors
#include <minwindef.h>

typedef typeof(((PFLT_PARAMETERS)NULL)->DirectoryControl.QueryDirectory) QUERY_DIRECTORY,
    *PQUERY_DIRECTORY;

extern PFLT_FILTER gFilterHandle;
extern CONST FLT_REGISTRATION FilterRegistration;

NTSTATUS FLTAPI FilterUnload(IN FLT_FILTER_UNLOAD_FLAGS Flags);

FLT_POSTOP_CALLBACK_STATUS PostDirectoryControl(IN OUT PFLT_CALLBACK_DATA Data,
                                                IN PCFLT_RELATED_OBJECTS FltObjects,
                                                IN OPTIONAL PVOID CompletionContext,
                                                IN FLT_POST_OPERATION_FLAGS Flags);

BOOL ShouldHideFile(IN LPCWSTR lpFileName, IN ULONG ulFileNameLength);

#endif  // __FILTER_H__
