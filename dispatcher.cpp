#include "ntddk.h"
#include "dispatcher.h"
#include "handleDeviceCtl.h"

NTSTATUS Dispatcher(PDEVICE_OBJECT, PIRP pIrp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION pStackLoc = IoGetCurrentIrpStackLocation(pIrp);

    switch (pStackLoc->MajorFunction) {
    case IRP_MJ_CREATE:
        KdPrint(("Cereate Request"));
        break;

    case IRP_MJ_READ:
        KdPrint(("Read Request"));
        break;

    case IRP_MJ_CLOSE:
        KdPrint(("Close Request"));
        break;

    case IRP_MJ_WRITE:
        KdPrint(("write request"));
        break;

    case IRP_MJ_DEVICE_CONTROL:
        KdPrint(("Device control request"));
        status = handleDeviceControl(pIrp);
    default:
        break;
    }

    
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);

    return status;
}