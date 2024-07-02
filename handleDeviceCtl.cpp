#include "ntddk.h"

#include "dispatcher.h"
#include "handleDeviceCtl.h"
#include "temperature.h"
#include "common.h"

extern KSEMAPHORE sem;

extern KSPIN_LOCK mylock;

extern BOOLEAN stopmon;

NTSTATUS handleDeviceControl(PIRP pIrp)
{
    KdPrint(("handleDeviceIoControl called!"));
    NTSTATUS status = STATUS_SUCCESS;

    PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(pIrp);
    KIRQL oirql = { 0 };

    ULONG ulRetLen = 0;
    //both input and ooutput bbuffers will be present in this buffer
    PVOID pRetBuffer = pIrp->AssociatedIrp.SystemBuffer;
    
    //ULONG ulInputLen = pIoStack->Parameters.DeviceIoControl.InputBufferLength;
    
    LARGE_INTEGER interval = { 0 };
    interval.QuadPart = (LONGLONG)(2 * 60);


    switch (pIoStack->Parameters.DeviceIoControl.IoControlCode) {

    case IOCTL_INIT_MON:
        // Do something
        InitThreadRoutine();
        ///   spin some kind of thread that starts tracking the temperature
        KdPrint(("spin some kind of thread that starts tracking the temperature"));

        break;

    case IOCTL_DUMP_TEMP:
        // DO something 
        KeReleaseSemaphore(&sem, 0, THREAD_COUNT, FALSE);

        while (1) {
            KeDelayExecutionThread(KernelMode, FALSE, &interval);

            if (GetTotalExecutionDone() == THREAD_COUNT) {
                break;
            }
        }

        GetDataIntoBuffer(pRetBuffer, ulRetLen);

        /// Signal to dump the temperature at the moment.
        KdPrint(("Signal to dump the temperature at the moment"));

        break;

    case IOCTL_DINIT_MON:
        KeAcquireSpinLock(&mylock, &oirql);
        stopmon = true; // signal to stop
        KeReleaseSpinLock(&mylock, oirql);

        KdPrint(("Signalled to stop"));
        break;


    default:
        status = STATUS_INVALID_PARAMETER;
        KdPrint(("Invalid parameter found!!\n"));
    }


    KdPrint(("handleDeviceIoControl ending with retLen %u, status 0x%x!",ulRetLen));

    pIoStack->Parameters.DeviceIoControl.OutputBufferLength = ulRetLen;
    pIrp->IoStatus.Information = ulRetLen;
    pIrp->IoStatus.Status = status;

    //// TODO

    IoCancelIrp(pIrp);

    return status;
}