#include "stdafx.h"
#include "LoadDll.h"
#include "LoadDllDlg.h"
#include "afxdialogex.h"
#include "disasm/LDE64.h"
#include "MemoryModule.h"


//
// Length Disassembler Engine by BeatriX
//
#ifdef _WIN64
#pragma comment(lib, "disasm/LDE64x64.lib")
#else
#pragma comment(lib, "disasm/LDE64.lib")
#endif

//
// Typedefs for functions.
//
typedef int (__stdcall *ProcNoArgStdcall)( );
typedef int (__stdcall *Proc1ArgStdcall)( void* );
typedef int (__stdcall *Proc2ArgStdcall)( void*, void* );
typedef int (__stdcall *Proc3ArgStdcall)( void*, void*, void* );
typedef int (__stdcall *Proc4ArgStdcall)( void*, void*, void*, void* );
typedef int (__stdcall *Proc5ArgStdcall)( void*, void*, void*, void*, void* );

typedef int (__fastcall *ProcNoArgFastcall)( );
typedef int (__fastcall *Proc1ArgFastcall)( void* );
typedef int (__fastcall *Proc2ArgFastcall)( void*, void* );
typedef int (__fastcall *Proc3ArgFastcall)( void*, void*, void* );
typedef int (__fastcall *Proc4ArgFastcall)( void*, void*, void*, void* );
typedef int (__fastcall *Proc5ArgFastcall)( void*, void*, void*, void*, void* );

typedef int (__cdecl *ProcNoArgCdeclcall)( );
typedef int (__cdecl *Proc1ArgCdeclcall)( void* );
typedef int (__cdecl *Proc2ArgCdeclcall)( void*, void* );
typedef int (__cdecl *Proc3ArgCdeclcall)( void*, void*, void* );
typedef int (__cdecl *Proc4ArgCdeclcall)( void*, void*, void*, void* );
typedef int (__cdecl *Proc5ArgCdeclcall)( void*, void*, void*, void*, void* );

typedef int (__thiscall *ProcNoArgThiscall)( );
typedef int (__thiscall *Proc1ArgThiscall)( void* );
typedef int (__thiscall *Proc2ArgThiscall)( void*, void* );
typedef int (__thiscall *Proc3ArgThiscall)( void*, void*, void* );
typedef int (__thiscall *Proc4ArgThiscall)( void*, void*, void*, void* );
typedef int (__thiscall *Proc5ArgThiscall)( void*, void*, void*, void*, void* );



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLoadDllDlg::CLoadDllDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoadDllDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLoadDllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EXPORTS, lstExports);
	DDX_Control(pDX, IDC_CMB_NUMBEROFARGS, cbNumberOfArgs);
	DDX_Control(pDX, IDC_ARG1, edtArg1);
	DDX_Control(pDX, IDC_ARG2, edtArg2);
	DDX_Control(pDX, IDC_ARG3, edtArg3);
	DDX_Control(pDX, IDC_ARG4, edtArg4);
	DDX_Control(pDX, IDC_ARG5, edtArg5);
	DDX_Control(pDX, ID_RUN, btnRun);
	DDX_Control(pDX, IDC_LOADFILE, btnLoadFile);
	DDX_Control(pDX, IDC_DLLPATH, edtDllPath);
	DDX_Control(pDX, IDC_CALL_EXPORT, rbExportedFunction);
	DDX_Control(pDX, IDC_CALL_ENTRYPOINT, rbCallEntrypoint);

	CButton rbCallEntrypoint;
	CButton rbExportedFunction;
	DDX_Control(pDX, IDC_CB_ARG1, cbArgType1);
	DDX_Control(pDX, IDC_CB_ARG2, cbArgType2);
	DDX_Control(pDX, IDC_CB_ARG3, cbArgType3);
	DDX_Control(pDX, IDC_CB_ARG4, cbArgType4);
	DDX_Control(pDX, IDC_CB_ARG5, cbArgType5);
	DDX_Control(pDX, IDC_CB_CALLINGCONV, cbCallingConvention);
	DDX_Control(pDX, IDC_PAUSE, m_chkPause);
}

BEGIN_MESSAGE_MAP(CLoadDllDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CALL_ENTRYPOINT, &CLoadDllDlg::OnBnClickedCallEntrypoint)
	ON_BN_CLICKED(IDC_CALL_EXPORT, &CLoadDllDlg::OnBnClickedCallExport)
	ON_CBN_SELCHANGE(IDC_CMB_NUMBEROFARGS, &CLoadDllDlg::OnCbnSelchangeCmbNumberofargs)
	ON_BN_CLICKED(ID_RUN, &CLoadDllDlg::OnBnClickedRun)
	ON_NOTIFY(NM_CLICK, IDC_LIST_EXPORTS, &CLoadDllDlg::OnNMClickListExports)
	ON_BN_CLICKED(IDC_LOADFILE, &CLoadDllDlg::OnBnClickedLoadfile)
	ON_BN_CLICKED(IDHELP, &CLoadDllDlg::OnBnClickedHelp)
END_MESSAGE_MAP()



#define ORDINAL_STR			"<BY_ORDINAL>"

//
// Dialog initialization.
//
BOOL CLoadDllDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//SetIcon(m_hIcon, TRUE);	
	SetIcon(m_hIcon, FALSE);

	//
	// Set up List View
	//
	lstExports.InsertColumn(0, "ID", LVCFMT_LEFT, 100 );
	lstExports.InsertColumn(1, "Function Name", LVCFMT_LEFT, 200);
	lstExports.InsertColumn(2, "Ordinal", LVCFMT_LEFT, 100);
	lstExports.InsertColumn(3, "Arguments", LVCFMT_LEFT, 60);
	lstExports.InsertColumn(4, "Function type", LVCFMT_LEFT, 200);
	lstExports.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);

	//
	// Set Components
	//
	rbCallEntrypoint.SetCheck( TRUE );
	EnableControls( FALSE );
	cbNumberOfArgs.SelectString( 0, "None" );
	cbCallingConvention.SelectString( 0, "Stdcall" );
	cbArgType1.SelectString( 0, "DWORD" );
	cbArgType2.SelectString( 0, "DWORD" );
	cbArgType3.SelectString( 0, "DWORD" );
	cbArgType4.SelectString( 0, "DWORD" );
	cbArgType5.SelectString( 0, "DWORD" );
	edtArg1.SetWindowTextA( "0" );
	edtArg2.SetWindowTextA( "0" );
	edtArg3.SetWindowTextA( "0" );
	edtArg4.SetWindowTextA( "0" );
	edtArg5.SetWindowTextA( "0" );

	//
	// Align column width
	//
	for( int x = 0; x < 4; x++ ) lstExports.SetColumnWidth( x, LVSCW_AUTOSIZE_USEHEADER );


	//Set up the tooltip
	m_pToolTip = new CToolTipCtrl;
	if(!m_pToolTip->Create(this))
	{
		return TRUE;
	}

	m_pToolTip->SetMaxTipWidth(500);
	m_pToolTip->AddTool(this, "LoadDLL by Esmid Idrizovic");
	m_pToolTip->AddTool(&m_chkPause,"This will call an INT 3 right before the DLL is loaded or the function executed, so don't worry if it looks like LoadDLL has crashed. You can attach with your debugger to LoadDLL and move on with your analysis.");
	m_pToolTip->AddTool(&rbCallEntrypoint, "This will load the DLL directly into memory with a custom LoadLibrary function and will execute all TLS entries and then the DllMain.");
	m_pToolTip->AddTool(&rbExportedFunction, "This will load the DLL using LoadLibrary and will execute the selected function.");
	m_pToolTip->AddTool(&btnLoadFile, "Select your DLL file...");
	m_pToolTip->AddTool(&cbCallingConvention, "You must select the right calling convention of the function.\n\nStdcall: function is going to clean up the stack.\nFastcall: ECX+EDX, rest on stack and function is going to clean up.\nCdecl: you have to clean up the stack.\nThiscall: this-pointer is in ECX, function is going to clean up the stack.");
	m_pToolTip->AddTool(&cbNumberOfArgs, "You can always change the number of the arguments if it's wrong detected by LoadDLL.");

	m_pToolTip->Activate(TRUE);


	return TRUE;
}

void CLoadDllDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CLoadDllDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//
// Enable/Disable argument-input controls.
//
void CLoadDllDlg::EnableControls( BOOL fState )
{
	edtArg1.EnableWindow( fState );
	edtArg2.EnableWindow( fState );
	edtArg3.EnableWindow( fState );
	edtArg4.EnableWindow( fState );
	edtArg5.EnableWindow( fState );
	cbArgType1.EnableWindow( fState );
	cbArgType2.EnableWindow( fState );
	cbArgType3.EnableWindow( fState );
	cbArgType4.EnableWindow( fState );
	cbArgType5.EnableWindow( fState );
	cbNumberOfArgs.EnableWindow( fState );
	cbCallingConvention.EnableWindow( fState );
	lstExports.EnableWindow( fState );
}

//
// Call entry point from DLL.
//
void CLoadDllDlg::OnBnClickedCallEntrypoint()
{
	EnableControls( FALSE );
	btnRun.SetWindowTextA( "Load DLL" );
	lstExports.DeleteAllItems();
}

//
// Call exported functions.
//
void CLoadDllDlg::OnBnClickedCallExport()
{
	EnableControls( TRUE );
	btnRun.SetWindowTextA( "Execute function" );
	EnumerateExportedFunctions();
	OnCbnSelchangeCmbNumberofargs();
}

//
// Enable only needed arguments for function call.
//
void CLoadDllDlg::OnCbnSelchangeCmbNumberofargs()
{
	int nCurSel = cbNumberOfArgs.GetCurSel();

	if ( nCurSel >= 0 )
	{
		edtArg1.EnableWindow( FALSE );
		edtArg2.EnableWindow( FALSE );
		edtArg3.EnableWindow( FALSE );
		edtArg4.EnableWindow( FALSE );
		edtArg5.EnableWindow( FALSE );

		if ( nCurSel >= 1 )
			edtArg1.EnableWindow( TRUE );

		if ( nCurSel >= 2 )
			edtArg2.EnableWindow( TRUE );

		if ( nCurSel >= 3 )
			edtArg3.EnableWindow( TRUE );

		if ( nCurSel >= 4 )
			edtArg4.EnableWindow( TRUE );

		if ( nCurSel >= 5 )
			edtArg5.EnableWindow( TRUE );

		cbArgType1.EnableWindow( edtArg1.IsWindowEnabled() );
		cbArgType2.EnableWindow( edtArg2.IsWindowEnabled() );
		cbArgType3.EnableWindow( edtArg3.IsWindowEnabled() );
		cbArgType4.EnableWindow( edtArg4.IsWindowEnabled() );
		cbArgType5.EnableWindow( edtArg5.IsWindowEnabled() );
	}
}

//
// Execute function/Load DLL clicked.
//
void CLoadDllDlg::OnBnClickedRun()
{
	if ( rbCallEntrypoint.GetCheck() )
	{
		LoadDll();
	}
	else if ( rbExportedFunction.GetCheck() )
	{
		LoadDllAndExecuteFunction();
	}
}

//
// Returns file offset from virtual address.
//
DWORD64 CLoadDllDlg::GetFOffsetFromRVA( LPBYTE pBuffer, WORD wNumberOfSections, DWORD64 dwAddr )
{
	if ( pBuffer == NULL || dwAddr == 0 )
		return (DWORD64)-1;

	IMAGE_SECTION_HEADER *pSection;
	DWORD64 dwResult = (DWORD64)-1;

	pSection = (IMAGE_SECTION_HEADER *)(pBuffer + ((IMAGE_DOS_HEADER*)pBuffer)->e_lfanew + sizeof(IMAGE_NT_HEADERS));
	for ( WORD n = 0; n < wNumberOfSections; n++ )
	{
		if ( dwAddr >= pSection->VirtualAddress && dwAddr < pSection->VirtualAddress + pSection->Misc.VirtualSize )
		{
			dwResult = (dwAddr - pSection->VirtualAddress) + pSection->PointerToRawData;
			break;
		}
		pSection++;
	}

	return dwResult;
}

//
// Enumerates all exports from a DLL.
//
BOOL CLoadDllDlg::EnumerateExportedFunctions()
{
	int nItem = 0;
	char szFileName[MAX_PATH +1];
	char szTemp[ 100 ];
	char * lpszFunctionName = NULL;
	UINT16 wOrdinal = 0, wNumberOfArgs = 0;

	PBYTE pFile = NULL;
	DWORD dwFileSize = 0;
	HANDLE hFile = NULL;
	DWORD dwBytesRead = 0;
	IMAGE_DOS_HEADER* pDos = NULL;
	IMAGE_NT_HEADERS* pNt = NULL;
	IMAGE_EXPORT_DIRECTORY* pExport = NULL;
	DWORD64 dwExportAddress, dwAddressOfFunctions, dwAddressOfNames, dwAddressOfNamesOrdinals, dwFunctionNameOffset, dwFunctionAddress;
	DWORD* lpszExportNameTable = NULL;
	WORD* lpExportOrdinalTable = NULL;
	DWORD* lpExportFunctionTable = NULL;
	WORD wNumberOfPseudoPushArguments = 0;

	lstExports.DeleteAllItems();
	edtDllPath.GetWindowTextA( szFileName, MAX_PATH );

	hFile = CreateFileA( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}

	dwFileSize = GetFileSize( hFile, NULL );
	if ( dwFileSize == 0 || dwFileSize == INVALID_FILE_SIZE )
		goto lblAbort;

	pFile = new BYTE[ dwFileSize ];

	if ( ReadFile(hFile, pFile, dwFileSize, &dwBytesRead, NULL) == FALSE || dwBytesRead != dwFileSize )
		goto lblAbort;

	pDos = (IMAGE_DOS_HEADER*)pFile;
	if ( pDos->e_magic != IMAGE_DOS_SIGNATURE )
		goto lblAbort;

	pNt = (IMAGE_NT_HEADERS*)((char*)pFile + pDos->e_lfanew);
	if ( pNt->Signature != IMAGE_NT_SIGNATURE )
		goto lblAbort;

	//
	// Are there any exported functions?
	//
	if ( pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0 || 
		 pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 )
		goto lblAbort;

	dwExportAddress = GetFOffsetFromRVA( pFile, pNt->FileHeader.NumberOfSections, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress );
	if ( dwExportAddress == (DWORD64)-1 )
		goto lblAbort;

	pExport = (IMAGE_EXPORT_DIRECTORY *)((char*)pFile + dwExportAddress);
	
	dwAddressOfFunctions = GetFOffsetFromRVA( pFile, pNt->FileHeader.NumberOfSections, pExport->AddressOfFunctions );
	dwAddressOfNames = GetFOffsetFromRVA( pFile, pNt->FileHeader.NumberOfSections, pExport->AddressOfNames );
	dwAddressOfNamesOrdinals = GetFOffsetFromRVA( pFile, pNt->FileHeader.NumberOfSections, pExport->AddressOfNameOrdinals );
	
	if ( dwAddressOfFunctions == (DWORD64)-1 || dwAddressOfNames == (DWORD64)-1 || dwAddressOfNamesOrdinals == (DWORD64)-1 )
		goto lblAbort;

	lpszExportNameTable = (DWORD*)( (char*)pFile + dwAddressOfNames );
	lpExportOrdinalTable = (WORD*)( (char*)pFile + dwAddressOfNamesOrdinals );
	lpExportFunctionTable = (DWORD*)( (char*)pFile + dwAddressOfFunctions );

	nItem = lstExports.GetItemCount();

	for ( DWORD n = 0; n < pExport->NumberOfNames; n++ )
	{
		//
		// Get exported function name
		//
		dwFunctionNameOffset = (DWORD)lpszExportNameTable[ n ];
		dwFunctionNameOffset = GetFOffsetFromRVA( pFile, pNt->FileHeader.NumberOfSections, dwFunctionNameOffset );

		if ( dwFunctionNameOffset == (DWORD64)-1 )
			lpszFunctionName = "UNKNOWN!";
		else
			lpszFunctionName = (char*)((char*)pFile + dwFunctionNameOffset);

		//
		// Get exported ordinal number
		//
		wOrdinal = lpExportOrdinalTable[ n ];
		dwFunctionAddress = lpExportFunctionTable[ wOrdinal ];

		//
		// Get number of arguments
		//
		dwFunctionAddress = GetFOffsetFromRVA( pFile, pNt->FileHeader.NumberOfSections, dwFunctionAddress );
		if ( dwFunctionAddress == (DWORD64)-1  )
		{
			//
			// Something went wrong, Lol.
			//
			wNumberOfArgs = 0;
		}
		else
		{
			//
			// disassemble our function buffer and find out whats the return code
			//
			PBYTE pPtr = (PBYTE)((char*)pFile + dwFunctionAddress);
			wNumberOfArgs = GetArgNumberFromFunction( pPtr, wNumberOfPseudoPushArguments );
		}

		//
		// Add index
		//
		wsprintfA( szTemp, "%d", nItem );
		if ( lstExports.InsertItem(LVIF_TEXT, nItem, szTemp, 0, 0, 0, NULL) != (int)-1 )
		{
			//
			// Add function name
			//
			lstExports.SetItemText( nItem, 1, lpszFunctionName );

			//
			// if the function name starts with an @ we assume its a fastcall, so we have to increment the number of arguments to atleast 2 (eax, ecx are used for first two arguments)
			//
			if ( lpszFunctionName[0] == '@' && wNumberOfArgs != (UINT16)-1 )
				wNumberOfArgs += 2;

			//
			// Add ordinal
			//
			wsprintfA( szTemp, "%d", wOrdinal + pExport->Base );
			lstExports.SetItemText( nItem, 2, szTemp );

			//
			// Add number of arguments
			//
			wsprintfA( szTemp, "%d", wNumberOfArgs != (UINT16)-1 ? wNumberOfArgs : 0 );
			lstExports.SetItemText( nItem, 3, szTemp );

			//
			// Add assumed function type, we can just guess the type
			//
			if ( wNumberOfArgs == 0 )
			{
				if (  wNumberOfPseudoPushArguments == 0 )
				{
					lstExports.SetItemText(nItem, 4, "Cdecl");
				}
				else
				{
					wsprintf( szTemp, "Cdecl but found %u push/mov argument[n] in the function", wNumberOfPseudoPushArguments );
					lstExports.SetItemText(nItem, 4, szTemp);
				}
			}
			else if ( lpszFunctionName[0] == '@' )
			{
				lstExports.SetItemText(nItem, 4, "Fastcall");
			}
			else if ( wNumberOfArgs == (UINT16)-1 )
			{
				lstExports.SetItemText(nItem, 4, "Exported Variable");
			}
			else
			{
				lstExports.SetItemText(nItem, 4, "Stdcall");
			}
			nItem++;
		}
	}

lblAbort:
 	delete[] pFile;
	CloseHandle( hFile );

	//
	// Align column width
	//
	for( int x = 0; x < 4; x++ ) lstExports.SetColumnWidth( x, LVSCW_AUTOSIZE_USEHEADER );

	return TRUE;
}

//
// Reads arguments from Text boxes and put them to pData and pArgPtr.
//
BOOL CLoadDllDlg::GetArgument( CEdit * pControl, CComboBox * pComboBox, LPBYTE pData, size_t nLength, PBYTE& pArgPtr )
{
	char szTemp[ 0x100 ];
	size_t nRet;

	//
	// pControl = Argument
	// pComboBox = Argument Type
	// 
	if ( pControl == NULL || pComboBox == NULL || pData == NULL || nLength == 0 )
		return FALSE;

	pArgPtr = NULL;

	nRet = pControl->GetWindowTextA( szTemp, sizeof(szTemp) );
	if ( nRet == 0 )
		return FALSE;

	nLength = __min(nLength, nRet + 1);

	if ( pComboBox->GetCurSel() == -1 )
	{
		//
		// Nothing selected
		//
		pComboBox->SetFocus();
		return FALSE;
	}
	else if ( pComboBox->GetCurSel() == 0 )
	{
		//
		// DWORD, convert string to number
		//
		DWORD dwValue = 0;
		if ( szTemp[0] == '0' && (szTemp[1] == 'x' || szTemp[1] == 'X') )
		{
			//
			// Hex
			//
			dwValue = strtol( &szTemp[2], (char**)(&szTemp + nLength - 1), 16 );
		}
		else
		{
			dwValue = atol(szTemp);
		}

		pArgPtr = (PBYTE)dwValue;
	}
	else if ( pComboBox->GetCurSel() == 1 )
	{
		//
		// STRING
		//
		memcpy( pData, szTemp, nLength );
		pArgPtr = pData;
	}
	else if ( pComboBox->GetCurSel() == 2 )
	{
		//
		// UNICODE STRING
		//
		if ( MultiByteToWideChar(CP_ACP, 0, szTemp, -1, (LPWSTR)pData, (int)nLength) > 0 )
		{
			pArgPtr = pData;
		}
	}
	else if ( pComboBox->GetCurSel() == 3 )
	{
		//
		// POINTER TO EMPTY MEMORY ALLOCATION
		//
		memset( pData, 0, nLength );
		pArgPtr = pData;

	}

	return TRUE;
}

BOOL LoadFileInMemory(const char* szFileName, LPBYTE &pBuffer, DWORD &dwSize)
{
	HANDLE hFile;
	DWORD dwBytesRead, dwFileSize;
	BOOL fResult = FALSE;

	hFile = CreateFileA( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		return fResult;
	}

	dwFileSize = GetFileSize( hFile, NULL );
	if ( dwFileSize == 0 || dwFileSize == INVALID_FILE_SIZE )
		goto lblAbort;

	pBuffer = new BYTE[ dwFileSize ];

	if ( ReadFile(hFile, pBuffer, dwFileSize, &dwBytesRead, NULL) == FALSE || dwBytesRead != dwFileSize )
		goto lblAbort;

	fResult = TRUE;
	dwSize = dwFileSize;

lblAbort:
	CloseHandle(hFile);
	return fResult;
}


//
// Load DLL and execute main entry point.
//
BOOL CLoadDllDlg::LoadDll( )
{
	HMEMORYMODULE hDLL;
	char szError[ 0x100 ] = { 0 };
	char szFileName[MAX_PATH +1] = { 0 };
	BOOL fResult = FALSE;
	LPBYTE pBuffer = NULL;
	DWORD dwSize = 0;

	//
	// Load DLL and execute main entry point.
	//
	edtDllPath.GetWindowTextA( szFileName, MAX_PATH );

	//
	// do a pause before executed DLL
	//
	if (!LoadFileInMemory(szFileName, pBuffer, dwSize))
	{
		goto lblAbort;
	}

	hDLL = MemoryLoadLibrary(pBuffer, m_chkPause.GetCheck() ? TRUE : FALSE);
	if ( hDLL == NULL )
	{
		wsprintfA( szError, "Can't load DLL \"%s\". Error = %d\n", szFileName, GetLastError() );
		this->MessageBox( szError, "Error", MB_ICONERROR );
		goto lblAbort;
	}

	this->MessageBox( "DLL loaded. Press OK to unload.", NULL, MB_ICONINFORMATION );

lblAbort:
	if ( hDLL != NULL )
	{
		MemoryFreeLibrary(hDLL);
		fResult = TRUE;
	}
	delete[] pBuffer;
	pBuffer = NULL;

	return fResult;
}

//
// Load DLL and execute exported function.
//
BOOL CLoadDllDlg::LoadDllAndExecuteFunction()
{
#define ARG_ALLOC_SIZE				0x100

	BOOL fResult = FALSE;
	HMODULE hDLL = NULL;
	char szErrorText[ 0x100 ];
	char szFileName[MAX_PATH +1];
	char szFunctionName[0x100] = { 0 };

	BYTE baArg1[ ARG_ALLOC_SIZE ] = { 0 };
	BYTE baArg2[ ARG_ALLOC_SIZE ] = { 0 };
	BYTE baArg3[ ARG_ALLOC_SIZE ] = { 0 };
	BYTE baArg4[ ARG_ALLOC_SIZE ] = { 0 };
	BYTE baArg5[ ARG_ALLOC_SIZE ] = { 0 };
	PBYTE pArg1, pArg2, pArg3, pArg4, pArg5;
	PVOID pFunction = NULL;

	//
	// Load DLL
	//
	edtDllPath.GetWindowTextA( szFileName, MAX_PATH );
	hDLL = LoadLibraryA( szFileName );
	if ( hDLL == NULL )
	{
		wsprintfA( szErrorText, "Can't load DLL \"%s\". Error = %d\n", szFileName, GetLastError() );
		this->MessageBox( szErrorText, "Error", MB_ICONERROR );
		goto lblAbort;
	}

	//
	// Get selected function. If ordinal get ordinal number (decimal).
	//
	if ( !GetTextFromListView(szFunctionName, sizeof(szFunctionName), 1) )
		goto lblAbort;

	if ( _stricmp(szFunctionName, ORDINAL_STR) == 0 )
	{
		if ( !GetTextFromListView(szFunctionName, sizeof(szFunctionName), 2) )
			goto lblAbort;

		DWORD dwOrdinal = atol(szFunctionName);
		pFunction = (PVOID)GetProcAddress( hDLL, (LPCSTR)dwOrdinal );
	}
	else
	{
		pFunction = (PVOID)GetProcAddress( hDLL, szFunctionName );
	}

	//
	// Check if function can be found.
	//
	if ( pFunction == NULL )
	{
		wsprintfA( szErrorText, "Can't find exported function \"%s\" in file \"%s\". Error = %d\n", szFunctionName, szFileName, GetLastError() );
		this->MessageBox( szErrorText, "Error", MB_ICONERROR );
		goto lblAbort;
	}

	//
	// Get all arguments.
	//
	GetArgument( &edtArg1, &cbArgType1, baArg1, sizeof(baArg1), pArg1 );
	GetArgument( &edtArg2, &cbArgType2, baArg2, sizeof(baArg2), pArg2 );
	GetArgument( &edtArg3, &cbArgType3, baArg3, sizeof(baArg3), pArg3 );
	GetArgument( &edtArg4, &cbArgType4, baArg4, sizeof(baArg4), pArg4 );
	GetArgument( &edtArg5, &cbArgType5, baArg4, sizeof(baArg5), pArg5 );

	ExecuteFunction( pFunction, pArg1, pArg2, pArg3, pArg4, pArg5 );

lblAbort:
	//
	// Unload library.
	//
	if ( hDLL != NULL )
	{
		if ( FreeLibrary(hDLL) == FALSE )
		{
			wsprintfA( szErrorText, "Can't unload DLL. Error = %d\n", GetLastError() );
			this->MessageBox( szErrorText, "Error", MB_ICONERROR );
		}
		else
		{
			fResult = TRUE;
		}
	}

	return fResult;
}

//
// Reads selected list view item to lpszBuffer.
//
BOOL CLoadDllDlg::GetTextFromListView( char * lpszBuffer, int nBufferSize, int nSubItem )
{
	LVITEM lvi = { 0 };

	if ( lpszBuffer == NULL || nBufferSize == 0 )
		return FALSE;

	int nItem = lstExports.GetNextItem( (int)-1, LVNI_SELECTED );
	if ( nItem == (int)-1 )
		return FALSE;

	lvi.iItem = nItem;
	lvi.iSubItem = nSubItem;
	lvi.mask = LVIF_TEXT;
	lvi.pszText = lpszBuffer;
	lvi.cchTextMax = nBufferSize;
	if ( !lstExports.GetItem(&lvi) )
	{
		*lpszBuffer = NULL;
		return FALSE;
	}

	return TRUE;
}

//
// Executes __stdcall function from a DLL.
//
void CLoadDllDlg::ExecuteFunction( PVOID pFunction, PBYTE pArg1, PBYTE pArg2, PBYTE pArg3, PBYTE pArg4, PBYTE pArg5 )
{
	int nFunctionReturn = 0;
	BOOL fException = FALSE;
	int nNumberOfArgsSelection;
	int nCallingConvention;

	//
	// Set last error to zero and execute the function
	//
	SetLastError( 0 );
	nNumberOfArgsSelection = cbNumberOfArgs.GetCurSel();
	nCallingConvention = cbCallingConvention.GetCurSel();

	if ( nNumberOfArgsSelection == 0 )
	{
		//
		// Function with no parameters
		//
		ProcNoArgStdcall	pFunctionStdcall =  (ProcNoArgStdcall)pFunction;
		ProcNoArgFastcall	pFunctionFastcall = (ProcNoArgFastcall)pFunction;
		ProcNoArgCdeclcall	pFunctionCdeclcall = (ProcNoArgCdeclcall)pFunction;
		ProcNoArgThiscall	pFunctionThiscall = (ProcNoArgThiscall)pFunction;

		//
		// call an INT3 if pause is checked
		//
		if ( m_chkPause.GetCheck() ) 
		{
			__debugbreak();
		}
		__try 
		{
			switch (nCallingConvention)
			{
				case 1:
					nFunctionReturn = pFunctionFastcall( );
					break;
				case 2:
					nFunctionReturn = pFunctionCdeclcall( );
					break;
				case 3:
					nFunctionReturn = pFunctionThiscall( );
					break;
				default:
					nFunctionReturn = pFunctionStdcall( );
					break;
			}
		}
		__except ( 1 )
		{ 
			fException = TRUE; 
		}
	}
	else if ( nNumberOfArgsSelection == 1 )
	{
		//
		// Function with 1 parameter
		//
		Proc1ArgStdcall		pFunctionStdcall =   (Proc1ArgStdcall)pFunction;
		Proc1ArgFastcall	pFunctionFastcall =  (Proc1ArgFastcall)pFunction;
		Proc1ArgCdeclcall	pFunctionCdeclcall = (Proc1ArgCdeclcall)pFunction;
		Proc1ArgThiscall	pFunctionThiscall =  (Proc1ArgThiscall)pFunction;

		//
		// call an INT3 if pause is checked
		//
		if ( m_chkPause.GetCheck() ) 
		{
			__debugbreak();
		}

		__try 
		{
			switch (nCallingConvention)
			{
			case 1:
				nFunctionReturn = pFunctionFastcall( pArg1 );
				break;
			case 2:
				nFunctionReturn = pFunctionCdeclcall( pArg1 );
				break;
			case 3:
				nFunctionReturn = pFunctionThiscall( pArg1 );
				break;
			default:
				nFunctionReturn = pFunctionStdcall( pArg1 );
				break;
			}
		}
		__except ( 1 )
		{ 
			fException = TRUE; 
		}
	}
	else if ( nNumberOfArgsSelection == 2 )
	{
		//
		// Function with 2 parameter
		//
		Proc2ArgStdcall		pFunctionStdcall =   (Proc2ArgStdcall)pFunction;
		Proc2ArgFastcall	pFunctionFastcall =  (Proc2ArgFastcall)pFunction;
		Proc2ArgCdeclcall	pFunctionCdeclcall = (Proc2ArgCdeclcall)pFunction;
		Proc2ArgThiscall	pFunctionThiscall =  (Proc2ArgThiscall)pFunction;

		//
		// call an INT3 if pause is checked
		//
		if ( m_chkPause.GetCheck() ) 
		{
			__debugbreak();
		}

		__try 
		{
			switch (nCallingConvention)
			{
			case 1:
				nFunctionReturn = pFunctionFastcall( pArg1, pArg2 );
				break;
			case 2:
				nFunctionReturn = pFunctionCdeclcall( pArg1, pArg2 );
				break;
			case 3:
				nFunctionReturn = pFunctionThiscall( pArg1, pArg2 );
				break;
			default:
				nFunctionReturn = pFunctionStdcall( pArg1, pArg2 );
				break;
			}
		}
		__except ( 1 )
		{ 
			fException = TRUE; 
		}
	}
	else if ( nNumberOfArgsSelection == 3 )
	{
		//
		// Function with 3 parameter
		//
		Proc3ArgStdcall		pFunctionStdcall =   (Proc3ArgStdcall)pFunction;
		Proc3ArgFastcall	pFunctionFastcall =  (Proc3ArgFastcall)pFunction;
		Proc3ArgCdeclcall	pFunctionCdeclcall = (Proc3ArgCdeclcall)pFunction;
		Proc3ArgThiscall	pFunctionThiscall =  (Proc3ArgThiscall)pFunction;

		//
		// call an INT3 if pause is checked
		//
		if ( m_chkPause.GetCheck() ) 
		{
			__debugbreak();
		}

		__try 
		{
			switch (nCallingConvention)
			{
			case 1:
				nFunctionReturn = pFunctionFastcall( pArg1, pArg2, pArg3 );
				break;
			case 2:
				nFunctionReturn = pFunctionCdeclcall( pArg1, pArg2, pArg3 );
				break;
			case 3:
				nFunctionReturn = pFunctionThiscall( pArg1, pArg2, pArg3 );
				break;
			default:
				nFunctionReturn = pFunctionStdcall( pArg1, pArg2, pArg3 );
				break;
			}
		}
		__except ( 1 )
		{ 
			fException = TRUE; 
		}
	}
	else if ( nNumberOfArgsSelection == 4 )
	{
		//
		// Function with 4 parameter
		//
		Proc4ArgStdcall		pFunctionStdcall =   (Proc4ArgStdcall)pFunction;
		Proc4ArgFastcall	pFunctionFastcall =  (Proc4ArgFastcall)pFunction;
		Proc4ArgCdeclcall	pFunctionCdeclcall = (Proc4ArgCdeclcall)pFunction;
		Proc4ArgThiscall	pFunctionThiscall =  (Proc4ArgThiscall)pFunction;

		//
		// call an INT3 if pause is checked
		//
		if ( m_chkPause.GetCheck() ) 
		{
			__debugbreak();
		}

		__try 
		{
			switch (nCallingConvention)
			{
			case 1:
				nFunctionReturn = pFunctionFastcall( pArg1, pArg2, pArg3, pArg4 );
				break;
			case 2:
				nFunctionReturn = pFunctionCdeclcall( pArg1, pArg2, pArg3, pArg4 );
				break;
			case 3:
				nFunctionReturn = pFunctionThiscall( pArg1, pArg2, pArg3, pArg4 );
				break;
			default:
				nFunctionReturn = pFunctionStdcall( pArg1, pArg2, pArg3, pArg4 );
				break;
			}
		}
		__except ( 1 )
		{ 
			fException = TRUE; 
		}
	}
	else if ( nNumberOfArgsSelection == 5 )
	{
		//
		// Function with 5 parameter
		//
		Proc5ArgStdcall		pFunctionStdcall =   (Proc5ArgStdcall)pFunction;
		Proc5ArgFastcall	pFunctionFastcall =  (Proc5ArgFastcall)pFunction;
		Proc5ArgCdeclcall	pFunctionCdeclcall = (Proc5ArgCdeclcall)pFunction;
		Proc5ArgThiscall	pFunctionThiscall =  (Proc5ArgThiscall)pFunction;

		//
		// call an INT3 if pause is checked
		//
		if ( m_chkPause.GetCheck() ) 
		{
			__debugbreak();
		}

		__try 
		{
			switch (nCallingConvention)
			{
			case 1:
				nFunctionReturn = pFunctionFastcall( pArg1, pArg2, pArg3, pArg4, pArg5 );
				break;
			case 2:
				nFunctionReturn = pFunctionCdeclcall( pArg1, pArg2, pArg3, pArg4, pArg5 );
				break;
			case 3:
				nFunctionReturn = pFunctionThiscall( pArg1, pArg2, pArg3, pArg4, pArg5 );
				break;
			default:
				nFunctionReturn = pFunctionStdcall( pArg1, pArg2, pArg3, pArg4, pArg5 );
				break;
			}
		}
		__except ( 1 )
		{ 
			fException = TRUE; 
		}
	}

	//
	// Show information about the executed function
	//
	ShowExecutedFunctionInformation(nFunctionReturn, fException);
}

//
// Show Information (Return Code/Last Error/Exception/...) about the executed function.
//
void CLoadDllDlg::ShowExecutedFunctionInformation( int nReturn, BOOL fException )
{
	char szText[ 0x100 ] = { 0 };
	char * lpszFormat = NULL;
	DWORD dwLastError = GetLastError();

	lpszFormat = fException ? 
		"Exception occurred. Please check again the arguments (PTR, ...).\n\n"
		"Return code: %08d (0x%08x)\nLast Error:  %08d (0x%08x)" : 
	"Return code: %08d (0x%08x)\nLast Error:  %08d (0x%08x)";

	wsprintfA( szText, lpszFormat, nReturn, nReturn, dwLastError, dwLastError );
	this->MessageBox( szText, fException ? "Exception" : "Function return", fException ? MB_ICONWARNING : MB_ICONINFORMATION );
}

//
// Disassemble function and get the number of arguments for a function.
//
UINT16 CLoadDllDlg::GetArgNumberFromFunction( PBYTE pAddress, WORD &wNumberOfPseudoPushArguments )
{
#ifdef _WIN64
	DWORD64 dwLength = 0, dwInstructions = 0;
#else
	DWORD dwLength = 0, dwInstructions = 0;
#endif

	int nDisasmType;
	UINT16 wNumberOfArgs = 0;

#ifdef _WIN64
	nDisasmType = 64;	// disassemble as x64 assembler in Win64 compile
#else
	nDisasmType = 32;	// disassemble as x32 assembler in Win32 compile
#endif

	wNumberOfPseudoPushArguments = 0;

	if ( *pAddress == 0x00 )
	{
		return (UINT16)-1;
	}

	while ( TRUE )
	{
		//
		// search for RETN, RET <UINT16>
		//
		if ( *pAddress == 0xC3 )
		{
#ifdef _WIN64
			wNumberOfArgs = 4;	// RCX, RDX, R8, R9
#endif
			break;
		}
		else if ( *pAddress == 0xC2 )
		{
			wNumberOfArgs = *(WORD*)(pAddress + 1);
			wNumberOfArgs /= sizeof(void*);
			break;
		}
		//
		// push [ebp +]
		//
		else if ( *pAddress == 0xFF && *(pAddress + 1) == 0x75 && *(pAddress + 2) >= 0x08 )
		{
			// push from stack ... looks like cdecl function but pushing args
			wNumberOfPseudoPushArguments++;
		}
		//
		// mov ecx, [ebp +]
		// mov eax, [ebp +]
		// mov edx, [ebp +]
		// mov ebx, [ebp +]
		//
		else if ( *pAddress == 0x8B && 
			(*(pAddress +1) == 0x45 || *(pAddress +1) == 0x4d || *(pAddress +1) == 0x55 || *(pAddress+1) == 0x5d) 
			&& *(pAddress + 2) >= 0x08 )
		{
			wNumberOfPseudoPushArguments++;
		}
		else if ( dwInstructions > 0x400 || *pAddress == 0x00 )
		{
			//
			// still nothing found? exit
			//
			break;
		}
		else if ( *pAddress == 0xCC )
		{
			break;
		}
		dwLength = LDE( (DWORD64)pAddress, nDisasmType );
		if ( dwLength == (DWORD64)-1 )
			break;

		dwInstructions++;
		pAddress = (PBYTE)((DWORD64)pAddress + dwLength);
	}

	return wNumberOfArgs;
}

//
// List View item clicked.
//
void CLoadDllDlg::OnNMClickListExports(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	if ( pNMItemActivate )
	{
		char szArgs[ 20 ];
		if ( GetTextFromListView( szArgs, sizeof(szArgs), 3 ) )
		{
			DWORD dwNum = atol(szArgs);
			if ( dwNum > 5 )
				strcpy_s( szArgs, sizeof(szArgs), "5" );

			if ( _stricmp(szArgs, "0") == 0 )
				strcpy_s( szArgs, sizeof(szArgs), "None" );



			char szType[100];
			if (GetTextFromListView(szType, sizeof(szType), 4))
			{
				if (_strnicmp("Stdcall", szType, 7) == 0)
					cbCallingConvention.SetCurSel(0);
				else if (_strnicmp("Fastcall", szType, 8) == 0)
					cbCallingConvention.SetCurSel(1);
				else if (_strnicmp("Cdecl", szType, 5) == 0)
				{
					cbCallingConvention.SetCurSel(2);
					char* szPos = strstr(szType, "Cdecl but found ");
					if ( szPos)
					{
						szPos += strlen("Cdecl but found ");
						const char* szTemp = strchr(szPos, ' ');
						memset(szArgs, 0, sizeof(szArgs));
						strncpy_s(szArgs, szPos, szTemp - szPos);
					}
				}
				else if ( _strnicmp("Thiscall", szType, 8) == 0 )
				{
					cbCallingConvention.SetCurSel(3);
				}
			}

			cbNumberOfArgs.SelectString( 0, szArgs );


			OnCbnSelchangeCmbNumberofargs();
		}

	}

	*pResult = 0;
}

//
// Show open dialog.
//
void CLoadDllDlg::OnBnClickedLoadfile()
{
	CFileDialog OpenDialog(TRUE, "DLL", "", OFN_FILEMUSTEXIST, 
		"DLL files (*.dll)|*.dll|All files (*.*)|*.*||", this);

	CString cFileName;
	BOOL fValidDLL = FALSE;

	OpenDialog.m_pOFN->lpstrTitle = "Select DLL file...";
	if( OpenDialog.DoModal() == IDOK )
	{
		//
		// Lets see if file is a valid DLL
		//
		cFileName = OpenDialog.GetPathName();
		fValidDLL = IsValidDLL( cFileName.GetString() );

		if ( fValidDLL )
		{
			edtDllPath.SetWindowTextA( cFileName.GetString() );
			if (  rbExportedFunction.GetCheck() )
			{
				OnBnClickedCallExport();
			}
			else if ( rbCallEntrypoint.GetCheck() )
			{
				OnBnClickedCallEntrypoint();
			}
		}
		else
		{
			EnableControls( FALSE );
		}

		rbExportedFunction.EnableWindow( fValidDLL );
		rbCallEntrypoint.EnableWindow( fValidDLL );
		if (rbExportedFunction.GetCheck())
			lstExports.EnableWindow( fValidDLL );
		btnRun.EnableWindow( fValidDLL );



	}


}

//
// Is file a valid DLL (PE in Win32 or PE+ in Win64).
//
BOOL CLoadDllDlg::IsValidDLL( const char * lpszFileName )
{
	HANDLE hFile;
	char szErrorText[ MAX_PATH + 0x100 ];
	BYTE baBuffer[ 0x1000 ] = { 0 };
	DWORD dwBytesRead;
	IMAGE_DOS_HEADER* pDos = NULL;
	IMAGE_NT_HEADERS* pNt = NULL;
	BOOL fResult = FALSE;

	hFile = CreateFileA( lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		wsprintfA( szErrorText, "Can't find file \"%s\".", lpszFileName );
		this->MessageBox( szErrorText, "Error", MB_ICONERROR );
		return fResult;
	}

	if ( ReadFile(hFile, &baBuffer, sizeof(baBuffer), &dwBytesRead, NULL) == FALSE )
	{
		wsprintfA( szErrorText, "Can't read DLL." );
		this->MessageBox( szErrorText, "Error", MB_ICONERROR );
		goto lblAbort;
	}

	pDos = (IMAGE_DOS_HEADER*)&baBuffer;
	if ( pDos->e_magic != IMAGE_DOS_SIGNATURE )
	{
		wsprintfA( szErrorText, "File is not a valid DLL." );
		this->MessageBox( szErrorText, "Error", MB_ICONERROR );
		goto lblAbort;
	}

	pNt = (IMAGE_NT_HEADERS*)((char*)&baBuffer + pDos->e_lfanew);
	if ( pNt->Signature != IMAGE_NT_SIGNATURE )
	{
		wsprintfA( szErrorText, "File is not a valid DLL." );
		this->MessageBox( szErrorText, "Error", MB_ICONERROR );
		goto lblAbort;
	}

#ifdef _WIN64
	if ( pNt->OptionalHeader.Magic != 0x20b )
	{
		wsprintfA( szErrorText, "You must use the x32 version for loading PE files." );
		this->MessageBox( szErrorText, "Error", MB_ICONERROR );
	}
#else
	if ( pNt->OptionalHeader.Magic != 0x10b )
	{
		wsprintfA( szErrorText, "You must use the x64 version for loading PE+ files." );
		this->MessageBox( szErrorText, "Error", MB_ICONERROR );
	}
#endif
	else if ( !(pNt->FileHeader.Characteristics & IMAGE_FILE_DLL) )
	{
		wsprintfA( szErrorText, "PE is not a DLL file" );
		this->MessageBox( szErrorText, "Error", MB_ICONERROR );
	}
	else
	{
		fResult = TRUE;
	}


lblAbort:
	CloseHandle( hFile );
	return fResult;

}


//
// Help
//
void CLoadDllDlg::OnBnClickedHelp()
{
	char * szText = "LoadDLL by Esmid Idrizovic, 23. Juni 2014\n\n"
					"Uses Length Disassembler Engine by BeatriX\n"
					"Uses MemoryModule by Joachim Bauch\n\n"
					"Arguments:\n"
					"DWORD: numeric parameter (start with \"0x\" for hex-numbers)\n"
					"STR: ASCII string as parameter\n"
					"STRW: Unicode string as parameter\n"
					"PTR: Empty memory space (256 bytes)\n\n"
					"WARNING:\n"
					"Don't load any suspected DLL on your real system! Use always an virtual machine!\n";

	this->MessageBox( szText, "Help", MB_ICONINFORMATION );
}


BOOL CLoadDllDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (NULL != m_pToolTip)
		m_pToolTip->RelayEvent(pMsg);

	return CDialogEx::PreTranslateMessage(pMsg);
}
