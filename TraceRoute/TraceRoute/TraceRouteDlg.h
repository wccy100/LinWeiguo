
// TraceRouteDlg.h : header file
//

#pragma once
#include "NIC.h"
#include "Device.h"
#include "EthernetFrame.h"
#include "ICMP.h"

#define WM_RECV (WM_USER + 100)
#define WM_HOST (WM_USER + 101)

// CTraceRouteDlg dialog
class CTraceRouteDlg : public CDialog
{
// Construction
public:
	CTraceRouteDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TRACEROUTE_DIALOG };

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
	afx_msg LRESULT OnHost(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

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
	CWinThread *hThread;
	BYTE ttl;
	SYSTEMTIME timesend;
	SYSTEMTIME timerecv;
	DWORD routerip;
	time_t timesub;

	CComboBox m_CComboBox;
	CListCtrl m_CListCtrl;
	CIPAddressCtrl m_CIPAddressCtrl;
public:
	afx_msg void OnBnClickedButtonAbout();
	afx_msg void OnBnClickedButtonTrace();
	afx_msg void OnCbnSelchangeCombo();
	void icmpRequest(BYTE *buffer, BYTE ttl);
	static UINT CTraceRouteDlg::recvsend(LPVOID lpParam);
	static UINT CTraceRouteDlg::hostOne(LPVOID lpParam);
};
