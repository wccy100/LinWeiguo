
// PortScannerDlg.h : header file
//

#pragma once
#include "NIC.h"
#include "Device.h"
#include "EthernetFrame.h"
#include "TCP.h"

#define WM_RECV (WM_USER + 100)

// CPortScannerDlg dialog
class CPortScannerDlg : public CDialog
{
// Construction
public:
	CPortScannerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PORTSCANNER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnRecv(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeCombo();
	afx_msg void OnBnClickedButtonScan();
	static UINT send(LPVOID lpParam);
	static UINT recv(LPVOID lpParam);
public:
	NIC nic;
	int index;
	Device device;
	bool started;
	DWORD m_dstip;
	DWORD m_srcip;
	DWORD m_mask;
	BYTE *m_srcmac;
	BYTE *m_dstmac;
	BYTE *m_gwmac;
	WORD m_sport;
	WORD m_eport;
	int count;
	WORD sp;

	CComboBox m_CComboBox;
	CListCtrl m_CListCtrl;
	CString m_startingport;
	CString m_endingport;
	CString m_ipaddress;
	UINT checked;
	CWinThread *hThreadsend;
	CWinThread *hThreadrecv;
};
