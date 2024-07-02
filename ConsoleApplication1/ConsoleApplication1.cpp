#include "Windows.h"
#include <iostream>
#include "fileapi.h"
#include "minwindef.h"

#define IOCTL_INIT_MON CTL_CODE (FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_READ_DATA )
#define IOCTL_DUMP_TEMP CTL_CODE (FILE_DEVICE_UNKNOWN, 0X802, METHOD_BUFFERED, FILE_WRITE_DATA )
#define IOCTL_DINIT_MON CTL_CODE (FILE_DEVICE_UNKNOWN, 0X803, METHOD_BUFFERED, FILE_READ_DATA )

typedef struct {
    UINT64 tempdata;
}cpudata, * pcpudata;

int openDevice();
int closeDevice();
int dumpTemp();

HANDLE handle = nullptr;

int main()
{
    std::cout << "Hello World!\n";

    std::cout << "Operation: \n1.Start getting temperature\n2. Dump the cpu temperature\n3. DeInit sensors\n10.Exit\n::::::" << std::endl;
    int choice = 0;
    std::cin >> choice;

    if (choice != 1 && choice != 0 && choice != 2 && choice != 10) {
        std::cout << "WrongChoice" << std::endl;
        return 0;
    }

    while (choice != 10) {
        switch (choice) {
        case 1:
            openDevice();
            break;
        case 2:
            dumpTemp();
            break;
        case 3:
            closeDevice();
            break;
        default:
            return -1;
        }
        std::cin >> choice;
    }
    return 0;
}

HANDLE createDriverHandle()
{
    HANDLE handle = nullptr;

    handle = CreateFile(L"\\\\.\\mydevicelink", GENERIC_ALL, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);

    if (INVALID_HANDLE_VALUE == handle) {
        return nullptr;
    }

    return handle;
}


int openDevice()
{
    handle = createDriverHandle();
    
    if (handle) {
        DWORD lpdw = 0;
        auto status = DeviceIoControl(handle, IOCTL_INIT_MON, NULL, 0, NULL, 0, &lpdw, NULL);

        std::cout << status << std::endl;

        std::cout << "Device IOCTL call made!!!!" << std::endl;

        if (handle) {
            CloseHandle(handle);
            std::cout << "Device handle CLOSED!!!" << std::endl;
            return 1;
        }
    }
    else {
        std::cout << "Handle not found!" << std::endl;
    }
    
    return 0;
}

int dumpTemp()
{
    handle = createDriverHandle();

    if (handle) {
        DWORD lpdw = 0;
        LPVOID buffer = nullptr;
        DWORD outSize = sizeof(cpudata);
        auto status = DeviceIoControl(handle, IOCTL_DUMP_TEMP, NULL, 0, buffer, outSize, &lpdw, NULL);

        pcpudata cpudata = (reinterpret_cast<pcpudata>(buffer));

        std::cout << status << std::endl;
        if (cpudata) {
            std::cout << cpudata->tempdata << std::endl;
        }

        std::cout << "Dump temp call crossed!!!!" << std::endl;

        if (handle) {
            CloseHandle(handle);
            std::cout << "Device handle CLOSED!!!" << std::endl;
        }
        return 1;
    } else 
    {
        std::cout << "Handle not found!" << std::endl;
    }
    return 0;
}

int closeDevice()
{
    auto handle = createDriverHandle();

    if (handle) {
        DWORD lpdw = 0;
        LPVOID buffer = nullptr;
        DWORD outsize = sizeof(cpudata);
        auto status = DeviceIoControl(handle, IOCTL_DINIT_MON, nullptr, 0, buffer, outsize, &lpdw, nullptr);

        std::cout << status << std::endl;

        if (handle) {
            CloseHandle(handle);
            std::cout << "Device handle CLOSED!!!" << std::endl;
        }
        return 1;
    } else {
        std::cout << "Handle not found!" << std::endl;
    }

    

    return 0;
}
