#include<windows.h>

BOOL WINAPI DllMain(
    HINSTANCE hModule,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
        MessageBox(NULL, "get hacked", "behehehehehehheheehehhe" , MB_OK) ; 

        break ; 
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}