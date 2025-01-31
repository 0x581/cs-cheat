#include "hijack_handle.hpp"

#include <iostream>
#include <iomanip>
#include <TlHelp32.h>
#include <string>
#include <Psapi.h>




//macros we use. Some can be found in wintrnl.h
#define SeDebugPriv 20
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004)
#define NtCurrentProcess ( (HANDLE)(LONG_PTR) -1 ) 
#define ProcessHandleType 0x7
#define SystemHandleInformation 16 


/*
STRUCTURES NEEDED FOR NTOPENPROCESS:
*/
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWCH   Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG           Length;
    HANDLE          RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG           Attributes;
    PVOID           SecurityDescriptor;
    PVOID           SecurityQualityOfService;
}  OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

typedef struct _CLIENT_ID
{
    PVOID UniqueProcess;
    PVOID UniqueThread;
} CLIENT_ID, * PCLIENT_ID;

/*
STRUCTURES NEEDED FOR HANDLE INFORMATION:
*/

typedef struct {
    ULONG ProcessId;
    BYTE ObjectTypeNumber;
    BYTE Flags;
    USHORT Handle;
    PVOID Object;
    ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, * PSYSTEM_HANDLE; //i shortened it to SYSTEM_HANDLE for the sake of typing

typedef struct {
    ULONG HandleCount;
    SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;

/*
FUNCTION PROTOTYPES:
*/
typedef NTSTATUS(NTAPI* _NtDuplicateObject)(
    HANDLE SourceProcessHandle,
    HANDLE SourceHandle,
    HANDLE TargetProcessHandle,
    PHANDLE TargetHandle,
    ACCESS_MASK DesiredAccess,
    ULONG Attributes,
    ULONG Options
    );

typedef NTSTATUS(NTAPI* _RtlAdjustPrivilege)(
    ULONG Privilege,
    BOOLEAN Enable,
    BOOLEAN CurrentThread,
    PBOOLEAN Enabled
    );

typedef NTSYSAPI NTSTATUS(NTAPI* _NtOpenProcess)(
    PHANDLE            ProcessHandle,
    ACCESS_MASK        DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PCLIENT_ID         ClientId
    );

typedef NTSTATUS(NTAPI* _NtQuerySystemInformation)(
    ULONG SystemInformationClass, //your supposed to supply the whole class but microsoft kept the enum mostly empty so I just passed 16 instead for handle info. Thats why you get a warning in your code btw
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );

//simple function i made that will just initialize our Object_Attributes structure as NtOpenProcess will fail otherwise
inline static OBJECT_ATTRIBUTES InitObjectAttributes(PUNICODE_STRING name, ULONG attributes, HANDLE hRoot, PSECURITY_DESCRIPTOR security)
{
    return OBJECT_ATTRIBUTES{
        .Length = sizeof(OBJECT_ATTRIBUTES),
        .RootDirectory = hRoot,
        .ObjectName = name,
        .Attributes = attributes,
        .SecurityDescriptor = security,
        // .SecurityQualityOfService = NULL,
    };
}

//the handles we will need to use later on
static HANDLE procHandle = NULL;
static HANDLE hProcess = NULL;
static HANDLE HijackedHandle = NULL;

//basic function i made that will get a proccess id from a binary name, you dont have to use it. It needs some rework but for now it gets the job done.
DWORD GetPID(const wchar_t* processName)
{
    DWORD pid = 0;

    PROCESSENTRY32W processEntry{};
    processEntry.dwSize = sizeof(PROCESSENTRY32W);

    const auto processSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (processSnapShot == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        std::cout << "Process snapshot failed: " << error << std::endl;

        return 0;
    }

    while (Process32NextW(processSnapShot, &processEntry))
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



inline static bool IsHandleValid(HANDLE handle) //i made this to simply check if a handle is valid rather than repeating the if statments
{
    return handle && handle != INVALID_HANDLE_VALUE;
}

static void CleanUpAndExit(const CHAR *ErrorMessage) //just a function to clean up and exit. 
{
    procHandle ? CloseHandle(procHandle) : 0;

    std::cout << ErrorMessage << std::endl;

    system("pause");

}

HANDLE HijackExistingHandle(DWORD dwTargetProcessId)
{
    // Get the base address of ntdll.dll
    const HMODULE Ntdll = GetModuleHandleW(L"ntdll");

    // Get the address of RtlAdjustPrivilege in ntdll.dll so we can grant our process the highest permission possible
    _RtlAdjustPrivilege RtlAdjustPrivilege = (_RtlAdjustPrivilege)GetProcAddress(Ntdll, "RtlAdjustPrivilege");

    boolean OldPriv; //store the old privileges

    // Give our program SeDeugPrivileges whcih allows us to get a handle to every process, even the highest privileged SYSTEM level processes.
    RtlAdjustPrivilege(SeDebugPriv, TRUE, FALSE, &OldPriv);

    //get the address of NtQuerySystemInformation in ntdll.dll so we can find all the open handles on our system
    _NtQuerySystemInformation NtQuerySystemInformation = (_NtQuerySystemInformation)GetProcAddress(Ntdll, "NtQuerySystemInformation");

    //get the address of NtDuplicateObject in ntdll.dll so we can duplicate an existing handle into our cheat, basically performing the hijacking
    _NtDuplicateObject NtDuplicateObject = (_NtDuplicateObject)GetProcAddress(Ntdll, "NtDuplicateObject");

    //get the address of NtOpenProcess in ntdll.dll so wecan create a Duplicate handle
    _NtOpenProcess NtOpenProcess = (_NtOpenProcess)GetProcAddress(Ntdll, "NtOpenProcess");


    //initialize the Object Attributes structure, you can just set each member to NULL rather than create a function like i did
    OBJECT_ATTRIBUTES Obj_Attribute = InitObjectAttributes(NULL, NULL, NULL, NULL);

    //clientID is a PDWORD or DWORD* of the process id to create a handle to
    CLIENT_ID clientID = { 0 };

    // Get all handles
    ULONG return_length = 0;

    NtQuerySystemInformation(SystemHandleInformation, NULL, 0, &return_length);

    return_length = (ULONG)(return_length * 1.1);
    return_length = 1024 * 1024 * 1024;

    auto hInfo = (SYSTEM_HANDLE_INFORMATION *) new byte[return_length];

    //we use this for checking if the Native functions succeed
    NTSTATUS NtRet = NtQuerySystemInformation(SystemHandleInformation, hInfo, return_length, NULL);

    if (!NT_SUCCESS(NtRet))
    {
        CleanUpAndExit("NtQuerySystemInformation Failed");
    }


    //a variable to store the number of handles OUR cheat has open.
    DWORD NumOfOpenHandles;

    //loop through each handle on our system, and filter out handles that are invalid or cant be hijacked
    for (size_t i = 0; i < hInfo->HandleCount; ++i)
    {
        //get the amount of outgoing handles OUR cheat has open
        GetProcessHandleCount(GetCurrentProcess(), &NumOfOpenHandles);

        //you can do a higher number if this is triggering false positives. Its just to make sure we dont fuck up and create thousands of handles
        if (NumOfOpenHandles > 500)
        {
            CleanUpAndExit("Error: Handle Leakage Detected");
        }

        //check if the current handle is valid, otherwise increment i and check the next handle
        if (!IsHandleValid((HANDLE)hInfo->Handles[i].Handle))
        {
            continue;
        }

        //check the handle type is 0x7 meaning a process handle so we dont hijack a file handle for example
        if (hInfo->Handles[i].ObjectTypeNumber != ProcessHandleType)
        {
            continue;
        }


        //set clientID to a pointer to the process with the handle to our target
        clientID.UniqueProcess = (DWORD*)hInfo->Handles[i].ProcessId;

        //if procHandle is open, close it
        procHandle ? CloseHandle(procHandle) : 0;

        //create a a handle with duplicate only permissions to the process with a handle to our target. NOT OUR TARGET.
        NtRet = NtOpenProcess(&procHandle, PROCESS_DUP_HANDLE | PROCESS_QUERY_INFORMATION, &Obj_Attribute, &clientID);
        if (!IsHandleValid(procHandle) || !NT_SUCCESS(NtRet)) //check is the funcions succeeded and check the handle is valid
        {
            continue;
        }


        //we duplicate the handle another process has to our target into our cheat with whatever permissions we want. I did all access.
        NtRet = NtDuplicateObject(procHandle, (HANDLE)hInfo->Handles[i].Handle, NtCurrentProcess, &HijackedHandle, PROCESS_ALL_ACCESS, 0, 0);
        if (!IsHandleValid(HijackedHandle) || !NT_SUCCESS(NtRet)) //check is the funcions succeeded and check the handle is valid
        {
            continue;
        }

        //get the process id of the handle we duplicated and check its to our target
        if (GetProcessId(HijackedHandle) != dwTargetProcessId) {
            CloseHandle(HijackedHandle);
            continue;
        }

        constexpr auto bSize = 2048;

        WCHAR buffer[bSize]{ L"A"};

        const auto ret = K32GetProcessImageFileNameW(procHandle, buffer, bSize);

        std::cout << "Handle: " << procHandle << ", pid: " << hInfo->Handles[i].ProcessId << " Ret: " << ret << std::endl;

        std::wcout << "Got handle from " << buffer << std::endl;

        hProcess = HijackedHandle;

        //break;
    }

    delete[] hInfo;

    CleanUpAndExit("Success");

    return hProcess;
}
