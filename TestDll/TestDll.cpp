// TestDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"




#ifdef __cplusplus
extern "C"
{
#endif
BOOL __declspec(dllexport) __stdcall CreateEmptyFileStdcall( LPCSTR lpszFileName, DWORD dwCharacters )
{
	if ( lpszFileName == NULL || dwCharacters < 1 )
		return FALSE;
	
	char szInfo[ MAX_PATH + 0x100 ];
	wsprintfA( szInfo, "FileName: %s\nNumber of chars: %d\n", lpszFileName, dwCharacters );
	MessageBoxA( 0, szInfo, "Arguments", 0 );

	HANDLE hFile = CreateFileA( lpszFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, 0 );
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		MessageBoxA( 0, "Can't create file", "Error", 0 );
		return FALSE;
	}

	DWORD dwWritten = 0;
	for ( DWORD n = 0; n < dwCharacters; n++ )
	{
		WriteFile( hFile, "A", 1, &dwWritten, NULL );
	}

	CloseHandle( hFile );

	return TRUE;
}

DWORD __declspec(dllexport) __fastcall ShowInfoFastCall( LPCSTR lpszText, LPCSTR lpszTitle, DWORD dwInfo )
{
	return MessageBoxA( 0, lpszText, lpszTitle, dwInfo );
}



DWORD __declspec(dllexport) __cdecl ShowInfoCdecl( LPCSTR lpszText, LPCSTR lpszTitle, DWORD dwInfo )
{
	return MessageBoxA( 0, lpszText, lpszTitle, dwInfo );
}


#ifdef __cplusplus
}
#endif

