#include "hijack_handle.hpp"

#include <iostream>
#include <iomanip>
#include <TlHelp32.h>
#include <string>
#include <Psapi.h>

#include "defines.hpp"

constexpr auto HANDLE_BUFFER_SIZE = 1024 * 1024 * sizeof(SYSTEM_HANDLE);
constexpr auto FILE_NAME_BUFFER_SIZE = 512;

constexpr auto MAX_HANDLE_COUNT = 512;



DWORD getPID(const wchar_t* processName)
{
    DWORD pid = 0;

    PROCESSENTRY32 processEntry{};
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    const auto processSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (processSnapShot == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        std::cout << "Process snapshot failed: " << error << std::endl;

        return 0;
    }

    while (Process32Next(processSnapShot, &processEntry))
    {
        if (wcscmp(processName, processEntry.szExeFile) == 0)
        {
            pid = processEntry.th32ProcessID;
            break;
        }
    }

    if (processSnapShot)
        CloseHandle(processSnapShot);

    return pid;
}



inline static bool isHandleValid(SYSTEM_HANDLE handle)
{
    return handle.Handle && reinterpret_cast<HANDLE>(handle.Handle) != INVALID_HANDLE_VALUE;
}

inline static bool isHandleValid(HANDLE handle)
{
    return handle && handle != INVALID_HANDLE_VALUE;
}

inline static bool isProcessHandle(SYSTEM_HANDLE handle)
{
    return handle.ObjectTypeNumber == ProcessHandleType;
}

static bool checkHandleCount()
{
    DWORD numOfOpenHandles;

    GetProcessHandleCount(GetCurrentProcess(), &numOfOpenHandles);

    return numOfOpenHandles > MAX_HANDLE_COUNT;
}

static bool checkProcessName(HANDLE hProcess, const wchar_t* processName)
{
    WCHAR buffer[FILE_NAME_BUFFER_SIZE]{ 0 };

    const auto processLength = GetProcessImageFileName(hProcess, buffer, FILE_NAME_BUFFER_SIZE);

    const auto targetProcessLength = wcslen(processName);

    std::wcout << "Checking " << buffer << std::endl;

    if (processLength < targetProcessLength)
    {
        return false;
    }

    return wcscmp(processName, &buffer[processLength - targetProcessLength]) == 0;
}

HANDLE hijackHandle(DWORD dwTargetProcessId, const wchar_t *fromProcess)
{
    if (dwTargetProcessId == 0)
    {
        return 0;
    }

    // Get the base address of ntdll.dll
    const HMODULE Ntdll = GetModuleHandle(L"ntdll");

    if (!Ntdll)
    {
        std::cout << "Failed to acquire Ntdll!" << std::endl;
        exit(-1);
    }

    // Get the address of RtlAdjustPrivilege in ntdll.dll so we can grant our process the highest permission possible
    _RtlAdjustPrivilege RtlAdjustPrivilege = (_RtlAdjustPrivilege)GetProcAddress(Ntdll, "RtlAdjustPrivilege");

    boolean OldPriv; 

    // Give our program SeDeugPrivileges whcih allows us to get a handle to every process, even the highest privileged SYSTEM level processes.
    NTSTATUS NtRet = RtlAdjustPrivilege(SeDebugPriv, TRUE, FALSE, &OldPriv);

    if (!NT_SUCCESS(NtRet))
    {
        std::cout << "Failed to adjust privilege level! (" << NtRet << ")" << std::endl;
        return 0;
    }

    _NtQuerySystemInformation NtQuerySystemInformation = (_NtQuerySystemInformation)GetProcAddress(Ntdll, "NtQuerySystemInformation");

    _NtDuplicateObject NtDuplicateObject = (_NtDuplicateObject)GetProcAddress(Ntdll, "NtDuplicateObject");

    _NtOpenProcess NtOpenProcess = (_NtOpenProcess)GetProcAddress(Ntdll, "NtOpenProcess");

    OBJECT_ATTRIBUTES Obj_Attribute{
        .Length = sizeof(OBJECT_ATTRIBUTES),
        .RootDirectory = 0,
        .ObjectName = 0,
        .Attributes = 0,
        .SecurityDescriptor = 0,
        .SecurityQualityOfService = 0,
    };

    auto hInfo = (SYSTEM_HANDLE_INFORMATION *) new byte[HANDLE_BUFFER_SIZE];

    // Query all open handles in the system
    NtRet = NtQuerySystemInformation(SystemHandleInformation, hInfo, HANDLE_BUFFER_SIZE, NULL);

    if (!NT_SUCCESS(NtRet))
    {
        std::cout << "Failed to query Handle Info. Perhaps HANDLE_BUFFER_SIZE is too small?" << std::endl;
        delete[] hInfo;
        return 0;
    }


    for (size_t i = 0; i < hInfo->HandleCount; ++i)
    {
        const auto system_handle = hInfo->Handles[i];

        if (checkHandleCount())
        {
            std::cout << "Error: Handle Leakage Detected. Handle count exceeded " << MAX_HANDLE_COUNT << "!" << std::endl;
            exit(-1);
        }

        if (!isHandleValid(system_handle) || !isProcessHandle(system_handle))
        {
            continue;
        }


        CLIENT_ID clientID{ .UniqueProcess = (PVOID)system_handle.ProcessId };
        HANDLE hProcess{ 0 };

        //create a a handle with duplicate only permissions to the process with a handle to our target. NOT OUR TARGET.
        NtRet = NtOpenProcess(&hProcess, PROCESS_DUP_HANDLE | PROCESS_QUERY_LIMITED_INFORMATION, &Obj_Attribute, &clientID);
        if (!NT_SUCCESS(NtRet) || !isHandleValid(hProcess))
        {
            continue;
        }


        HANDLE duplicatedHandle{ 0 };

        //we duplicate the handle another process has to our target into our cheat with whatever permissions we want. I did all access.
        NtRet = NtDuplicateObject(hProcess, (HANDLE)system_handle.Handle, GetCurrentProcess(), &duplicatedHandle, PROCESS_ALL_ACCESS, 0, 0);
        if (!NT_SUCCESS(NtRet) || !isHandleValid(duplicatedHandle))
        {
            CloseHandle(hProcess);
            continue;
        }

        if (GetProcessId(duplicatedHandle) != dwTargetProcessId || !checkProcessName(hProcess, fromProcess))
        {
            CloseHandle(hProcess);
            CloseHandle(duplicatedHandle);
            continue;
        }

        CloseHandle(hProcess);

        delete[] hInfo;
        return duplicatedHandle;
    }

    delete[] hInfo;

    std::cout << "No matching handles found..." << std::endl;
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwTargetProcessId);
    
    //return 0;
}


HANDLE hijackHandle(const wchar_t* targetProcess, const wchar_t* fromProcess)
{
    return hijackHandle(getPID(targetProcess), fromProcess);
}
