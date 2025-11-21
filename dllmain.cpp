// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
            return FALSE;

        if (!FindGameOffsets())
        {
            return FALSE;
        }
        CreateThread(
            NULL,
            0,
            (LPTHREAD_START_ROUTINE)HookOpenGL,
            0,
            0,
            NULL);
        return TRUE;
    }
    return FALSE;
}

