#include <ntifs.h>
#include <wdm.h>
// Include minwindef.h after ntifs.h and wdm.h to avoid redefinition errors
#include <minwindef.h>
#include "dispatch.h"
#include "mini_rootkit.h"
#include "structures.h"
#include "utils.h"

_Use_decl_annotations_ NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,
                                            IN PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status = STATUS_SUCCESS;

    MyDbgPrint("Loading...");
    DriverObject->DriverUnload = DriverUnload;

    NTSTATUS res = IoCreateDevice(DriverObject, 0, &DEVICE_NAME, FILE_DEVICE_UNKNOWN,
                                  FILE_DEVICE_SECURE_OPEN, FALSE, &DriverObject->DeviceObject);
    if (!NT_SUCCESS(res)) {
        MyDbgPrint("Failed to create device (error: %d)", res);
        goto ret_fail;
    }

    res = IoCreateSymbolicLink(&DEVICE_LINK, &DEVICE_NAME);
    if (!NT_SUCCESS(res)) {
        MyDbgPrint("Failed to create symbolic link (error: %d)", res);
        goto delete_dev;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchFunction;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchFunction;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchFunction;

    MyDbgPrint("Loaded");
    goto ret_success;

delete_dev:
    IoDeleteDevice(DriverObject->DeviceObject);

ret_fail:
    status = STATUS_UNSUCCESSFUL;

ret_success:
    return status;
}

_Use_decl_annotations_ VOID DriverUnload(IN PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
    MyDbgPrint("Unloading...");

    NTSTATUS res = IoDeleteSymbolicLink(&DEVICE_LINK);
    if (!NT_SUCCESS(res)) {
        MyDbgPrint("Failed to delete symbolic link (error: %d)", res);
    }
    IoDeleteDevice(DriverObject->DeviceObject);

    MyDbgPrint("Unloaded");
}
