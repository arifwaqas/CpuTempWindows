#pragma once


#define IOCTL_INIT_MON CTL_CODE (FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_READ_DATA )
#define IOCTL_DUMP_TEMP CTL_CODE (FILE_DEVICE_UNKNOWN, 0X802, METHOD_BUFFERED, FILE_WRITE_DATA )
#define IOCTL_DINIT_MON CTL_CODE (FILE_DEVICE_UNKNOWN, 0X803, METHOD_BUFFERED, FILE_READ_DATA )

#define THREAD_COUNT 1

static KSEMAPHORE sem;

static KSPIN_LOCK mylock;

static BOOLEAN stopmon = FALSE;

typedef struct {
    UINT64 tempdata;
}cpudata, * pcpudata;