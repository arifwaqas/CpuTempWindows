#include "ntddk.h"
#include "temperature.h"
#include "common.h"

extern KSEMAPHORE sem;

extern KSPIN_LOCK mylock;

extern BOOLEAN stopmon;

UINT64 finalCpuTemp = 0;
int totalExecutions = 0;
VOID SaveCalculation(UINT64 cpuTemp);
VOID ThreadProc(PVOID);

VOID InitThreadRoutine()
{
    KdPrint(("InitThreadRoutine called!"));

    NTSTATUS status = STATUS_SUCCESS;
    HANDLE thread[THREAD_COUNT] = { 0 };
    HANDLE threadobj[THREAD_COUNT] = { 0 };

    for (int i = 0; i < THREAD_COUNT; i++) {
        status = PsCreateSystemThread(&thread[i],0, NULL, NULL, NULL,ThreadProc,NULL);
        if (status != STATUS_SUCCESS)
        {
            KdPrint(("PsCreateSystemThread failed"));
            break;
        }

        //checks if the handle is valid and the access is applicable on our handle that we opened above
        status = ObReferenceObjectByHandle(thread[i], THREAD_ALL_ACCESS, NULL, KernelMode, &threadobj[i], NULL);
        if (!NT_SUCCESS(status))
        {
            KdPrint(("ObReferenceObjectByHandle failed"));
            break;
        }
        ZwClose(thread[i]);
    }
}


VOID ThreadProc(PVOID)
{
    // Logic to get the cpu temperature in the temp variable
    KdPrint(("ThreadProc called!"));
    UINT64 cputemp = 0;
    KIRQL oirql = { 0 };

    while (1) {
        KdPrint(("Waiting for semaphore!"));
        KeWaitForSingleObject(&sem, Executive, KernelMode, 0, NULL);
        KdPrint(("Sem signalled !"));


        KeAcquireSpinLock(&mylock, &oirql);
        bool isStopmon = stopmon;
        KeReleaseSpinLock(&mylock, oirql);

        if (isStopmon == TRUE)
        {
            KdPrint(("stopmon true!"));
            break;
        }

        //// ADD code to calculate CpuTemp
        cputemp = 69;
        SaveCalculation(cputemp);
        /// END code to calculate cpuTemp
        
        KeAcquireSpinLock(&mylock, &oirql);
        ++totalExecutions;
        KeReleaseSpinLock(&mylock, oirql);

        KdPrint(("semaphore SIGNALLED\n"));
    
    }

    PsTerminateSystemThread(STATUS_SUCCESS);
}


UINT64 GetTotalExecutionDone()
{
    KdPrint(("GetTotalExecutioknDone called!"));
    KIRQL oirql = { 0 };
    KeAcquireSpinLock(&mylock, &oirql);
    UINT64 total = totalExecutions;
    KeReleaseSpinLock(&mylock, oirql);

    return total;
}

VOID GetDataIntoBuffer(PVOID buffer, ULONG& len)
{
    KdPrint(("GetDataIntoBuffer called!"));
    pcpudata pdata = NULL;
    pdata = (pcpudata)buffer;
    RtlSecureZeroMemory(pdata, sizeof(cpudata));
    pdata->tempdata = finalCpuTemp;
    len = sizeof(cpudata);
    return;
}


VOID SaveCalculation(UINT64 cpuTemp)
{
    finalCpuTemp = cpuTemp;
}