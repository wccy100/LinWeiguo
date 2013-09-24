
// LANScanner.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LANScanner.h"
#include "LANScannerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLANScannerApp

BEGIN_MESSAGE_MAP(CLANScannerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLANScannerApp construction

CLANScannerApp::CLANScannerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CLANScannerApp object

CLANScannerApp theApp;


// CLANScannerApp initialization

BOOL CLANScannerApp::InitInstance()
{
	CWinAppEx::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CLANScannerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
