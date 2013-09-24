
// IPMonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IPMonitor.h"
#include "IPMonitorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CIPMonitorDlg dialog




CIPMonitorDlg::CIPMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIPMonitorDlg::IDD, pParent)
	, started(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIPMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_NIC, m_CComboBox);
	DDX_Control(pDX, IDC_LIST, m_CListCtrl);
	DDX_Control(pDX, IDC_CHECK_SOURCE, m_CheckBoxSource);
	DDX_Control(pDX, IDC_CHECK_DESTINATION, m_CheckBoxDestination);
	DDX_Text(pDX, IDC_EDIT_IPADDRESS, m_edit_ipaddress);
	DDX_Text(pDX, IDC_EDIT_SUBNETMASK, m_edit_subnetmask);
	DDX_Text(pDX, IDC_EDIT_MACADDRESS, m_edit_macaddress);
	DDX_Text(pDX, IDC_EDIT_GATEWAYIPADDRESS, m_edit_gatewayipaddress);
	DDX_Text(pDX, IDC_EDIT_GATEWAYMAC, m_edit_gatewaymac);
}

BEGIN_MESSAGE_MAP(CIPMonitorDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CIPMonitorDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CIPMonitorDlg::OnBnClickedButtonAbout)
	ON_CBN_SELCHANGE(IDC_COMBO_NIC, &CIPMonitorDlg::OnCbnSelchangeComboNic)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, &CIPMonitorDlg::OnBnClickedButtonScan)
	ON_MESSAGE(WM_RECV,OnRecv)
	ON_BN_CLICKED(IDC_CHECK_SOURCE, &CIPMonitorDlg::OnBnClickedCheckSource)
	ON_BN_CLICKED(IDC_CHECK_DESTINATION, &CIPMonitorDlg::OnBnClickedCheckDestination)
END_MESSAGE_MAP()


// CIPMonitorDlg message handlers

BOOL CIPMonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	index = -1;
	nic.initNIC();
	vector<char *> al = nic.getAdapterList();
	int row = 0;
	for(vector<char *>::const_iterator iter = al.begin(); iter!=al.end(); ++iter)
	{
		m_CComboBox.InsertString(row, *iter);
		++row;
	}
	m_CListCtrl.InsertColumn(0, _T("Source"), LVCFMT_LEFT, 95, -1);
    m_CListCtrl.InsertColumn(1, _T("Destination"), LVCFMT_LEFT, 120, -1);
    m_CListCtrl.InsertColumn(2, _T("Protocol"), LVCFMT_LEFT, 105, -1);
    m_CListCtrl.InsertColumn(3, _T("Packets"), LVCFMT_LEFT, 60, -1);
	srcchecked = m_CheckBoxSource.GetCheck();
	dstchecked = m_CheckBoxDestination.GetCheck();
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIPMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIPMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CIPMonitorDlg::OnBnClickedButtonExit()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}

void CIPMonitorDlg::OnBnClickedButtonAbout()
{
	// TODO: 在此添加控件通知处理程序代码
	MessageBox("Chenchao (113520081202014)\nEmail: orcish02@gmail.com\nSchool of Computing, CUC");
}

void CIPMonitorDlg::OnCbnSelchangeComboNic()
{
	// TODO: 在此添加控件通知处理程序代码
	index = m_CComboBox.GetCurSel();
	nic.selectNIC(index);
	m_edit_ipaddress = CString(nic.getIPAddress());
	localip = inet_addr(nic.getIPAddress());
	m_edit_subnetmask = CString(nic.getSubnetMask());
	m_edit_macaddress = CString(Common::mac2str(nic.getMacAddress()));
	m_edit_gatewayipaddress = CString(nic.getGatewayIPAddress());
	m_edit_gatewaymac = CString(Common::mac2str(nic.getGatewayMac()));
	UpdateData(FALSE);
}

UINT CIPMonitorDlg::recvAll(LPVOID lpParam)
{
	CIPMonitorDlg *p = (CIPMonitorDlg *)lpParam;
	pcap_t *adhandle = p->device.getHandle();
	int res;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	EthernetFrame ef;
	IP ip, *pip;
	PDU *pdu = &ip;
	ef.SetMACDATA(pdu);

	bool exp;
    /* Retrieve the packets */
    while((res = pcap_next_ex( adhandle, &header, &pkt_data)) >= 0)
	{
        if(res == 0)
            /* Timeout elapsed */
            continue;

		ef.Read((BYTE *)pkt_data);
		pip = dynamic_cast<IP *>(pdu);
		switch(p->flag)
		{
			case 0:
				exp = 1;
				break;
			case 1:
				exp = p->localip == pip->getSourceaddress();
				break;
			case 2:
				exp = p->localip == pip->getDestinationaddress();
				break;
			case 3:
				exp = p->localip == pip->getDestinationaddress() || p->localip == pip->getSourceaddress();
				break;
			default:
				exp = 0;
				break;
		}
		if(exp)
		{
			GlobalLock((HGLOBAL)&p);
			p->srcip = pip->getSourceaddress();
			p->dstip = pip->getDestinationaddress();
			p->proto = pip->getProtocol();
			AfxGetMainWnd()->SendMessage(WM_RECV, (WPARAM)p, 0);
		}
    }
    
    if(res == -1){
        return -1;
    }
	return 0;
}

LRESULT CIPMonitorDlg::OnRecv(WPARAM wParam, LPARAM lParam)
{
	CIPMonitorDlg *p = (CIPMonitorDlg *)wParam;

	CString ipsrc(Common::dword2char(p->srcip));
	CString ipdes(Common::dword2char(p->dstip));
	CString proto(Common::GetProtocol(p->proto));
    int count = m_CListCtrl.GetItemCount();
    for (int i = 0; i < count; i++)
    {
		if (ipsrc == m_CListCtrl.GetItemText(i, 0) && 
			ipdes == m_CListCtrl.GetItemText(i, 1) && 
			proto == m_CListCtrl.GetItemText(i, 2))
        {
            int pcount = atoi((LPCTSTR)m_CListCtrl.GetItemText(i,3));
            CString pcountstr;
            pcountstr.Format("%d", pcount+1);
            m_CListCtrl.SetItemText(i, 3, pcountstr);
            UpdateData(FALSE);
            return 0;
        }
    }

	m_CListCtrl.InsertItem(count, ipsrc);
	m_CListCtrl.SetItemText(count, 1, ipdes);
	m_CListCtrl.SetItemText(count, 2, proto);
	m_CListCtrl.SetItemText(count, 3, CString("1"));

	UpdateData(FALSE);
	GlobalUnlock((HGLOBAL)p);
	return 0;
}

void CIPMonitorDlg::OnBnClickedButtonScan()
{
	// TODO: 在此添加控件通知处理程序代码
	if(-1 == index)
		return;

	if (!started)
    {
		if(0==srcchecked && 0==dstchecked)
			flag = 0;
		else if(1==srcchecked && 0==dstchecked)
			flag = 1;
		else if(0==srcchecked && 1==dstchecked)
			flag = 2;
		else
			flag = 3;

        started = true;
        GetDlgItem(IDC_BUTTON_SCAN)->SetWindowText("&Stop");
        GetDlgItem(IDC_COMBO_NIC)->EnableWindow(FALSE);
        m_CListCtrl.DeleteAllItems();

		device.obtainDeviceList();
		device.openAdapter(nic.getNICname());
		string filter = "ip";
		device.setFilter(filter.c_str());

		hThreadrecv = AfxBeginThread(recvAll, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);

    }
    else
    {
        started = false;
        GetDlgItem(IDC_BUTTON_SCAN)->SetWindowText("&Scan");
        GetDlgItem(IDC_COMBO_NIC)->EnableWindow(TRUE);
		TerminateThread(hThreadrecv, NULL);
    }
}
void CIPMonitorDlg::OnBnClickedCheckSource()
{
	// TODO: 在此添加控件通知处理程序代码
	srcchecked = m_CheckBoxSource.GetCheck();
}

void CIPMonitorDlg::OnBnClickedCheckDestination()
{
	// TODO: 在此添加控件通知处理程序代码
	dstchecked = m_CheckBoxDestination.GetCheck();
}
