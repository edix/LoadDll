// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_PROCESS_DETACH:
		char szInfo[ 0x100 ];

		wsprintfA( szInfo, "DLL Loaded.\nhModule = %08x\nReason = %08x\nReserved = %08x\n", hModule, ul_reason_for_call, lpReserved );
		MessageBoxA( 0, szInfo, "Loaded", MB_ICONINFORMATION );
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}

