#include <windows.h>

#define DllExport   __declspec( dllexport )

VOID DllExport ShowBox(LPCWSTR title, LPCWSTR text)
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
        wchar_t temp[128];
        wsprintf(temp, L"hModule: 0x%p", hModule);
        ShowBox(L"Hoi!", (LPCWSTR)temp);
        break;
    case DLL_PROCESS_DETACH:
        ShowBox(L"Doei!", L"Mag niet.");
        break;
    }
    return TRUE;
}

