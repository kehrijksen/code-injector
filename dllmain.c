#include <windows.h>

#define DllExport   __declspec( dllexport )

VOID DllExport ShowBox(LPCSTR title, LPCSTR text)
{
    MessageBox(NULL, text, title, MB_OK);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ShowBox("DLL INJECTION", "ATTACH");
        break;
    case DLL_PROCESS_DETACH:
        ShowBox("DLL INJECTION", "DETACH");
        break;
    }
    return TRUE;
}

