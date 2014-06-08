
// LoadDll.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'stdafx.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CLoadDllApp:
// Siehe LoadDll.cpp für die Implementierung dieser Klasse
//

class CLoadDllApp : public CWinApp
{
public:
	CLoadDllApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CLoadDllApp theApp;