#include "filter.h"
#include <fltKernel.h>
#include <ntifs.h>
#include <wdm.h>
#include "utils.h"

PFLT_FILTER gFilterHandle = NULL;  // Global handle to register/unregister the filter

// List of callbacks for the filter operations
CONST FLT_OPERATION_REGISTRATION Callbacks[] = {
    {IRP_MJ_DIRECTORY_CONTROL, 0, NULL, PostDirectoryControl},
    {IRP_MJ_OPERATION_END},
};

CONST FLT_REGISTRATION FilterRegistration = {
    sizeof(FLT_REGISTRATION),
    FLT_REGISTRATION_VERSION,
    0,             // Flags
    NULL,          // Context
    Callbacks,     // Callback array of type FLT_OPERATION_REGISTRATION[]
    FilterUnload,  // Unload callback for this filter
    NULL,          // The remaining fields are NULL
};

/* Define the relevant file information structure offsets for `IRP_MN_QUERY_DIRECTORY`.
   Structures have different layouts, so we need to keep track of the field offsets.
   `FILE_INFORMATION_DEFINITION` records the Class and corresponding offsets for the fields we need:
    - NextEntry
    - FileName
    - FileNameLength

   https://learn.microsoft.com/en-us/previous-versions/windows/drivers/ifs/irp-mj-directory-control#parameters
   https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/flt-parameters-for-irp-mj-directory-control#members
*/
CONST FILE_INFORMATION_DEFINITION gFileInfoDefs[] = {
    FileBothDirectoryInformationDefinition, FileIdBothDirectoryInformationDefinition,
    FileFullDirectoryInformationDefinition, FileIdFullDirectoryInformationDefinition,
    FileDirectoryInformationDefinition,     FileNamesInformationDefinition,
};
// Number of file information definitions
#define G_FILE_INFO_DEFS_COUNT ARRAYSIZE(gFileInfoDefs)

NTSTATUS FLTAPI FilterUnload(IN FLT_FILTER_UNLOAD_FLAGS Flags) {
    UNREFERENCED_PARAMETER(Flags);
    if (gFilterHandle) {
        MyDbgPrint("Unloading filter...");
        FltUnregisterFilter(gFilterHandle);
        gFilterHandle = NULL;
    }
    return STATUS_SUCCESS;
}

FLT_POSTOP_CALLBACK_STATUS PostDirectoryControl(IN OUT PFLT_CALLBACK_DATA Data,
                                                IN PCFLT_RELATED_OBJECTS FltObjects,
                                                IN OPTIONAL PVOID CompletionContext,
                                                IN FLT_POST_OPERATION_FLAGS Flags) {
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);
    UNREFERENCED_PARAMETER(Flags);

    if (Data->Iopb->MinorFunction != IRP_MN_QUERY_DIRECTORY) {
        MyDbgPrint("PostDirectoryControl aborting: Not a directory query");
        goto ret;
    }
    if (!NT_SUCCESS(Data->IoStatus.Status)) {
        MyDbgPrint("PostDirectoryControl aborting: Status is unsuccessful (0x%08X), Class: %d",
                   Data->IoStatus.Status,
                   Data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileInformationClass);
        goto ret;
    }

    PQUERY_DIRECTORY pQueryDir = &Data->Iopb->Parameters.DirectoryControl.QueryDirectory;

    PVOID pBuffer = pQueryDir->DirectoryBuffer;
    if (pBuffer == NULL) {
        MyDbgPrint("PostDirectoryControl aborting: Buffer is NULL");
        goto ret;
    }

    ULONG ulLength = pQueryDir->Length;
    if (ulLength == 0) {
        MyDbgPrint("PostDirectoryControl aborting: Length is 0");
        goto ret;
    }

    ULONG_PTR ullInformation = Data->IoStatus.Information;
    if (ullInformation == 0) {
        MyDbgPrint("PostDirectoryControl aborting: Information is 0");
        goto ret;
    }

    // Find the correct FileInformationClass and associated offsets
    FILE_INFORMATION_CLASS fileInfoClass = pQueryDir->FileInformationClass;
    CONST FILE_INFORMATION_DEFINITION* pInfoDef = NULL;
    ULONG i;
    for (i = 0; i < G_FILE_INFO_DEFS_COUNT; ++i) {
        pInfoDef = &gFileInfoDefs[i];
        // MyDbgPrint(
        //     "gFileInfoDefs[%lu] %p: Class=%d, NextEntryOffset=%lu, FileNameOffset=%lu, "
        //     "FileNameLengthOffset=%lu",
        //     i, pInfoDef, pInfoDef->Class, pInfoDef->NextEntryOffset, pInfoDef->FileNameOffset,
        //     pInfoDef->FileNameLengthOffset);
        if (pInfoDef->Class == fileInfoClass) {
            MyDbgPrint("Found matching FileInformationClass for %d at index %lu", fileInfoClass, i);
            break;
        }
    }
    if (i == G_FILE_INFO_DEFS_COUNT) {
        MyDbgPrint("PostDirectoryControl aborting: No matching FileInformationClass found for %d",
                   fileInfoClass);
        goto ret;
    }

    PBYTE pInfo = (PBYTE)pBuffer;
    PBYTE pPrev = NULL, pNext = NULL;
    ULONG ulRemaining = 0;
    ULONG_PTR ullTotalLength = ullInformation;
    ULONG ulNextEntryOffset = 0;
    LPWSTR lpFileName = NULL;
    ULONG ulFileNameLengthB = 0, ulFileNameLengthCh = 0;
    ULONG ulEntryLength = 0;

    MyDbgPrint("PostDirectoryControl: Scanning buffer %p, Length: %lu, Information: %llu", pBuffer,
               ulLength, ullInformation);
    while (TRUE) {
        ulNextEntryOffset = FIELD_VALUE(pInfo, ULONG, pInfoDef->NextEntryOffset);
        lpFileName = (LPWSTR)(pInfo + pInfoDef->FileNameOffset);
        ulFileNameLengthB = FIELD_VALUE(pInfo, ULONG, pInfoDef->FileNameLengthOffset);  // in bytes
        ulFileNameLengthCh = ulFileNameLengthB / sizeof(*lpFileName);  // in characters
        ulEntryLength = MY_ALIGN_UP_PTR(pInfoDef->FileNameOffset + ulFileNameLengthB);

        MyDbgPrint("FileName: %.*ws, NextEntryOffset: %lu, Calculated size: %lu, TotalLength: %llu",
                   ulFileNameLengthCh, lpFileName, ulNextEntryOffset, ulEntryLength,
                   ullTotalLength);

        if (IsFileHidden(lpFileName, ulFileNameLengthCh)) {
            MyDbgPrint("> Hiding file %.*ws", ulFileNameLengthCh, lpFileName);
            if (ulNextEntryOffset == 0)  // Last entry
            {
                if (pPrev == NULL) {
                    MyDbgPrint("Removing the only entry entry");
                    ullTotalLength = 0;  // Set buffer length to 0
                } else {
                    MyDbgPrint("Removing last entry");
                    // Previous entry becomes the last
                    FIELD_VALUE(pPrev, ULONG, pInfoDef->NextEntryOffset) = 0;
                    ullTotalLength -= ulEntryLength;
                }
                break;
            } else {
                pNext = pInfo + ulNextEntryOffset;
                ulRemaining = (ULONG)((ULONG_PTR)pBuffer + ulLength - (ULONG_PTR)pNext);
                MyDbgPrint("Removing current entry: moving %lu bytes from %p to %p", ulRemaining,
                           pNext, pInfo);
                RtlMoveMemory(pInfo, pNext, ulRemaining);  // Move next entry to current position
                ullTotalLength -= ulNextEntryOffset;
            }
        } else {
            pPrev = pInfo;
            // Get next entry
            pInfo += ulNextEntryOffset;
        }
        if (ulNextEntryOffset == 0) {  // Stop if this is the last entry
            break;
        }
    }

    // Update the buffer length if we removed any entries
    if (ullTotalLength < ullInformation) {
        MyDbgPrint("Updating buffer length: %llu -> %llu", ullInformation, ullTotalLength);
        Data->IoStatus.Information = ullTotalLength;
        if (ullTotalLength == 0) {
            // Set status to indicate no files were found
            // (otherwise the caller actually sees the hidden file in the buffer)
            MyDbgPrint("We removed all entries: setting status to STATUS_NO_SUCH_FILE");
            Data->IoStatus.Status = STATUS_NO_SUCH_FILE;
        }
    }

ret:
    return FLT_POSTOP_FINISHED_PROCESSING;
}
