
// LANScannerDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "NIC.h"
#include "Device.h"
#include "EthernetFrame.h"
#include "PDU.h"
#include "ARP.h"
#include "afxcmn.h"

#define WM_COMPLETE (WM_USER + 100)
#define WM_HOST (WM_USER + 101)

// CLANScannerDlg dialog
class CLANScannerDlg : public CDialog
{
// Construction
public:
	CLANScannerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LANSCANNER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnComplete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHost(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	NIC nic;
	int index;
	Device device;
	BYTE dstmac[6];
	BYTE srcmac[6];
	DWORD localip;
	DWORD netmask;
	DWORD ipbeg;
	DWORD ipend;
	bool started;
	CWinThread *hThreadsend;
	CWinThread *hThreadrecv;
	SYSTEMTIME timesend;
	SYSTEMTIME timerecv;
	time_t diff;
	DWORD hostip;
	BYTE hostmac[6];

	CComboBox m_CComboBox;
	CListCtrl m_CListCtrl;
	CString m_edit_ipaddress;
	CString m_edit_subnetmask;
	CString m_edit_macaddress;
	CString m_edit_gatewayipaddress;
	CString m_edit_gatewaymac;
public:
	afx_msg void OnCbnSelchangeComboNic();
	afx_msg void OnBnClickedButtonScan();
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonAbout();
	static UINT sendAll(LPVOID lpParam);
	static UINT recvAll(LPVOID lpParam);
	static UINT hostOne(LPVOID lpParam);
};
