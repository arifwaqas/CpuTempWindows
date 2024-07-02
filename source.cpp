#include "ntddk.h"
#include "common.h"
#include "dispatcher.h"

extern KSEMAPHORE sem;

extern KSPIN_LOCK mylock;

extern BOOLEAN stopmon;

PDEVICE_OBJECT deviceObject = NULL;
UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\mydevice");
UNICODE_STRING symLinkName = RTL_CONSTANT_STRING(L"\\??\\mydevicelink");

VOID Unload(PDRIVER_OBJECT);


extern "C" 
NTSTATUS DriverEntry(PDRIVER_OBJECT driverObject, PUNICODE_STRING)
{
    KdPrint(("Reeached here"));
    NTSTATUS status = STATUS_SUCCESS;

    driverObject->DriverUnload = Unload;

    KeInitializeSemaphore(&sem, 0, THREAD_COUNT);

    //KeInitializeSpinLock(&mylock);

    status = IoCreateDevice(driverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &deviceObject);

    if (!NT_SUCCESS(status)) {
        KdPrint(("Device creation failed\n"));
        return status;
    }
    
    status = IoCreateSymbolicLink(&symLinkName, &deviceName);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Symlink creation failed\n"));
        IoDeleteDevice(deviceObject);
        return status;
    }

    KdPrint(("Driver laoded successfully!\n"));

    //////////////////////////////

    for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
        driverObject->MajorFunction[i] = Dispatcher;
    }

    return status;
}


VOID Unload(PDRIVER_OBJECT)
{
    KeReleaseSemaphore(&sem, 0, THREAD_COUNT, FALSE);

    IoDeleteSymbolicLink(&symLinkName);

    IoDeleteDevice(deviceObject);

    KdPrint(("Driver successfully unloaded\n"));

    return;
}