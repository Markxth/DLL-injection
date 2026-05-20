#include <windows.h> 
#include <stdio.h>
#include<ntstatus.h> 
#define STATUS_SUCCESS ((NTSTATUS)0X000000000L) //ntsatus success 

//----------FUNCTION PROTOTYPES START------------------
//0x30 bytes (sizeof)
typedef struct _OBJECT_ATTRIBUTES
{
    ULONG Length;                                                           //0x0
    VOID* RootDirectory;                                                    //0x8
    struct _UNICODE_STRING* ObjectName;                                     //0x10
    ULONG Attributes;                                                       //0x18
    VOID* SecurityDescriptor;                                               //0x20
    VOID* SecurityQualityOfService;                                         //0x28
}OBJECT_ATTRIBUTES, *PCOBJECT_ATTRIBUTES ; ; 

//0x10 bytes (sizeof)
typedef struct _CLIENT_ID
{
    VOID* UniqueProcess;                                                    //0x0
    VOID* UniqueThread;                                                     //0x8
}CLIENT_ID, *PCLIENT_ID; 

typedef struct _PS_ATTRIBUTE
{
    ULONG_PTR Attribute;
    SIZE_T Size;
    union
    {
        ULONG_PTR Value;
        PVOID ValuePtr;
    };
    PSIZE_T ReturnLength;
} PS_ATTRIBUTE, *PPS_ATTRIBUTE;
typedef struct _PS_ATTRIBUTE_LIST
{
    SIZE_T TotalLength;
    PS_ATTRIBUTE Attributes[1];
} PS_ATTRIBUTE_LIST, *PPS_ATTRIBUTE_LIST;
typedef NTSTATUS(NTAPI* NtCreateThreadEx)( //NT API of CreateRemoteThread()
    _Out_ PHANDLE ThreadHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ProcessHandle,
    _In_ PVOID StartRoutine,
    _In_opt_ PVOID Argument,
    _In_ ULONG CreateFlags, // THREAD_CREATE_FLAGS_*
    _In_ SIZE_T ZeroBits,
    _In_ SIZE_T StackSize,
    _In_ SIZE_T MaximumStackSize,
    _In_opt_ PPS_ATTRIBUTE_LIST AttributeList
    ); 
typedef NTSTATUS(NTAPI* NtWriteVirtualMemory) ( //NT version of WriteProcessMemory 
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID BaseAddress,
    _In_reads_bytes_(NumberOfBytesToWrite) PVOID Buffer,
    _In_ SIZE_T NumberOfBytesToWrite,
    _Out_opt_ PSIZE_T NumberOfBytesWritten
) ; 

typedef NTSTATUS(NTAPI* NtOpenProcess)(
    _Out_ PHANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PCLIENT_ID ClientId 
);

typedef NTSTATUS(NTAPI* NtClose)(
    _In_ _Post_ptr_invalid_ HANDLE Handle
) ; 

typedef NTSTATUS(NTAPI* NtAllocateVirtualMemory){
     _In_ HANDLE ProcessHandle,
    _Inout_ _At_(*BaseAddress, _Readable_bytes_(*RegionSize) _Writable_bytes_(*RegionSize) _Post_readable_byte_size_(*RegionSize)) PVOID *BaseAddress,
    _In_ ULONG_PTR ZeroBits,
    _Inout_ PSIZE_T RegionSize,
    _In_ ULONG AllocationType,
    _In_ ULONG PageProtection
}

//---------FUNCTION PROTOTYPES END-----------------

//------------DEFINE REUSABLE FUNCTIONS------------------

HMODULE getH(IN LPCWSTR modName) { 
    HMODULE hModuse = NULL ; 
    hModuse =  GetModuleHandleW(modName);
    if ( hModuse == NULL){ 
        printf("Could not get a handle to : %S\n", modName) ;
        return NULL  ; 
    }
    else { 
        printf("Got a handle : %S , at memory address : %p\n", modName, hModuse) ; 
        return hModuse ; 
    }

    //function prototypes 

}

NtOpenProcess    dckzOpen   = NULL; 
NtCreateThreadEx dckzCreate = NULL; 
NtClose          dckzClose  = NULL; 
NtWriteVirtualMemory dckzWrite = NULL;
NtAllocateVirtualMemory dckzAlloc = NULL; 


HANDLE hProcess = 0 ; 
DWORD PID  = 0 ;
HMODULE hKernel32 = NULL;  
wchar_t* dllPath = L"C:\\Users\\markt\\Downloads\\dllinj\\inj.dll" ;  //define path to DLL 

int main(int argc , char* argv[] ){ 
size_t dllPathSize = (wcslen(dllPath) + 1) * sizeof(wchar_t);
    if (argc < 2 ) {
        printf("Not 'nuff variables big man , not nuff...*read in a cowboy tone* \n ") ; 
        return EXIT_FAILURE ;
    }
PID = atoi(argv[1]);

HMODULE hNtdll = getH(L"ntdll.dll");

NtOpenProcess dckzOpen = (NtOpenProcess)GetProcAddress(hNtdll, "NtOpenProcess") ; 
if(dckzOpen == NULL){
    printf("Failed to get NtOpenProcess from ntdll.dll: %p\n", hNtdll ) ; 
    return EXIT_FAILURE; 
}
else{
    printf("Got handle for NtOpenProcess at : %p\n", dckzOpen) ; 
 }

dckzCreate = (NtCreateThreadEx)GetProcAddress(hNtdll, "NtCreateThreadEx" ) ; 
if(dckzCreate == NULL){ 
    printf("Failed to get NtCreateThreadEx for : %p\n", hNtdll ) ; 
    return EXIT_FAILURE; 
}
else{
    printf("Got handle for NtCreateThreadEx at : %p\n", dckzCreate) ; 
 }

dckzClose = (NtClose)GetProcAddress(hNtdll, "NtClose") ; 
if(dckzClose == NULL){ 
    printf("Failed to get NtClose for : %p\n", hNtdll ) ; 
    return EXIT_FAILURE; 
}
else{
    printf("Got handle for NtClose at : %p\n", dckzClose) ; 
 }

//---------use the stuff aka injct-------------

OBJECT_ATTRIBUTES OA = {
     sizeof(OA), 
     NULL //since we want the rest all to be NULL, we can do this 
} ; 

CLIENT_ID CID = {
    (HANDLE)(ULONG_PTR)PID,
    NULL
} ; 

dckzOpen(
    &hProcess,
    PROCESS_ALL_ACCESS, 
    //now, since OBJECT_ATTRIBUTES has its own characteristics we will have to intiialise this before ussing it here. 
    &OA,
    //same goes for CLIENT_ID l
    &CID
) ; 


 //OLD CODE 

//time to get the kernel32 module handle, using get module handle

hKernel32 = GetModuleHandleW(
    L"Kernel32.dll"
) ; 

if(hKernel32 == NULL){ 
    printf("Failed to get a handle to kernel32.dll  %lu\n", GetLastError() ) ; 
    return EXIT_FAILURE ;
}
    else{
        printf("Got a kernel32 handle at : %p\n", hKernel32) ; 
    }

LPTHREAD_START_ROUTINE startMain = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW") ; //as we want to use load library as that func just takes a DLL and runs it. 
printf("Got the adress of LoadLib at : %p\n ", startMain) ; 

//NT Virtual Alloc 
NTSTATUS status = dckzAlloc(

) ; 

//NT WriteProcMemo
}