#include <windows.h> 
#include <stdio.h>

HANDLE hProcess = 0 ; 
DWORD PID  = 0 ;
HMODULE hKernel32 = NULL;  
wchar_t* dllPath = L"C:\\Users\\markt\\Downloads\\dllinj\\inj.dll" ;  //define path to DLL 

int main(int argc , char* argv[] ){ 
size_t dllPathSize = (wcslen(dllPath) + 1) * sizeof(wchar_t);
    if (argc < 2 ) {
        printf("Not 'nuff variables big man , not nuff...*read in a cowboy tone* ") ; 
        return EXIT_FAILURE ;
    }
PID = atoi(argv[1]);
hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID ) ; 
if (hProcess == NULL) { 
    printf("Failed to open process : %lu\n", GetLastError()) ;
}
else { 
    printf("Got process : %lu\n", PID) ; 
}


LPVOID memo = VirtualAllocEx( 
    hProcess, 
    NULL,
    dllPathSize, 
    MEM_COMMIT | MEM_RESERVE,
    PAGE_READWRITE 
) ; 
//aloc write and then get a handle  - tbd tmrw ; 

BOOL result = WriteProcessMemory(
    hProcess,
    memo,
    dllPath, // bcs our dll is stored elsewhere, externally 
    dllPathSize ,
    NULL
) ; 

//virtualprotectex

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

HANDLE hThread = CreateRemoteThread(
    hProcess,
    NULL,
    0,
    startMain,
    memo,
    0,
    NULL
) ; 

if(hThread == NULL){ 
    printf("hThread Failed : %lu\n :", GetLastError() )  ; 
    return EXIT_FAILURE ;
}
else{
    printf("hThread created at : %p\n", hThread) ; 

}

WaitForSingleObject(hThread, INFINITE) ; 
printf("Thread finished executing") ;

CloseHandle(hThread) ;
CloseHandle(hProcess) ; 
return EXIT_SUCCESS ; 
}