
// LoadDllDlg.h: Headerdatei
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CLoadDllDlg-Dialogfeld
class CLoadDllDlg : public CDialogEx
{
// Konstruktion
public:
	CLoadDllDlg(CWnd* pParent = NULL);	// Standardkonstruktor
	~CLoadDllDlg()
	{
		delete m_pToolTip;
		m_pToolTip = NULL;
	}

// Dialogfelddaten
	enum { IDD = IDD_LOADDLL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	void EnableControls( BOOL fState );
public:
	afx_msg void OnBnClickedCallEntrypoint();
	CListCtrl lstExports;
	CComboBox cbNumberOfArgs;
	CEdit edtArg1;
	CEdit edtArg2;
	CEdit edtArg3;
	CEdit edtArg4;
	CEdit edtArg5;
	CButton btnRun;
	CButton btnLoadFile;
	CButton rbCallEntrypoint;
	CButton rbExportedFunction;
	afx_msg void OnBnClickedCallExport();
	afx_msg void OnCbnSelchangeCmbNumberofargs();
	afx_msg void OnBnClickedRun();

	BOOL EnumerateExportedFunctions();

	DWORD64 GetFOffsetFromRVA( LPBYTE pBuffer, WORD wNumberOfSections, DWORD64 dwAddr );
	UINT16 GetArgNumberFromFunction( PBYTE pAddress, WORD& wNumberOfPseudoPushArguments );
	BOOL GetArgument( CEdit * pControl, CComboBox * pComboBox, LPBYTE pData, size_t nLength, PBYTE& pArgPtr );
	BOOL LoadDll();
	BOOL LoadDllAndExecuteFunction();

	void ExecuteFunction( PVOID pFunction, PBYTE pArg1, PBYTE pArg2, PBYTE pArg3, PBYTE pArg4, PBYTE pArg5 );
	BOOL GetTextFromListView( char * lpszBuffer, int nBufferSize, int nSubItem = 1 );
	void ShowExecutedFunctionInformation( int nReturn, BOOL fException = FALSE );

	CEdit edtDllPath;
	CComboBox cbArgType1;
	CComboBox cbArgType2;
	CComboBox cbArgType3;
	CComboBox cbArgType4;
	CComboBox cbArgType5;
	afx_msg void OnNMClickListExports(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedLoadfile();

	BOOL IsValidDLL( const char * lpszFileName );
	BOOL LoadFile(const char* szFileName);
	afx_msg void OnBnClickedHelp();
	CComboBox cbCallingConvention;
	CButton m_chkPause;
	CToolTipCtrl* m_pToolTip;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
