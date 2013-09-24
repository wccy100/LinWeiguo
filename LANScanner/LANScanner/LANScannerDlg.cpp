
// LANScannerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LANScanner.h"
#include "LANScannerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD hostip2;
// CLANScannerDlg dialog

CLANScannerDlg::CLANScannerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLANScannerDlg::IDD, pParent)
	, started(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLANScannerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_NIC, m_CComboBox);
	DDX_Control(pDX, IDC_LIST, m_CListCtrl);
	DDX_Text(pDX, IDC_EDIT_IPADDRESS, m_edit_ipaddress);
	DDX_Text(pDX, IDC_EDIT_SUBNETMASK, m_edit_subnetmask);
	DDX_Text(pDX, IDC_EDIT_MACADDRESS, m_edit_macaddress);
	DDX_Text(pDX, IDC_EDIT_GATEWAYIPADDRESS, m_edit_gatewayipaddress);
	DDX_Text(pDX, IDC_EDIT_GATEWAYMAC, m_edit_gatewaymac);
}

BEGIN_MESSAGE_MAP(CLANScannerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_NIC, &CLANScannerDlg::OnCbnSelchangeComboNic)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, &CLANScannerDlg::OnBnClickedButtonScan)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CLANScannerDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CLANScannerDlg::OnBnClickedButtonAbout)
	ON_MESSAGE(WM_COMPLETE,OnComplete)
	ON_MESSAGE(WM_HOST,OnHost)
END_MESSAGE_MAP()


// CLANScannerDlg message handlers

BOOL CLANScannerDlg::OnInitDialog()
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
	m_CListCtrl.InsertColumn(0, _T("IP Address"), LVCFMT_LEFT, 95, -1);
    m_CListCtrl.InsertColumn(1, _T("MAC Address"), LVCFMT_LEFT, 120, -1);
    m_CListCtrl.InsertColumn(2, _T("Host Name"), LVCFMT_LEFT, 105, -1);
    m_CListCtrl.InsertColumn(3, _T("Arp Time"), LVCFMT_LEFT, 60, -1);
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLANScannerDlg::OnPaint()
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
HCURSOR CLANScannerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CLANScannerDlg::OnCbnSelchangeComboNic()
{
	// TODO: 在此添加控件通知处理程序代码
	index = m_CComboBox.GetCurSel();
	nic.selectNIC(index);
	m_edit_ipaddress = CString(nic.getIPAddress());
	m_edit_subnetmask = CString(nic.getSubnetMask());
	m_edit_macaddress = CString(Common::mac2str(nic.getMacAddress()));
	m_edit_gatewayipaddress = CString(nic.getGatewayIPAddress());
	m_edit_gatewaymac = CString(Common::mac2str(nic.getGatewayMac()));
	UpdateData(FALSE);
}

UINT CLANScannerDlg::sendAll(LPVOID lpParam)
{
	CLANScannerDlg *p = (CLANScannerDlg *)lpParam;
	EthernetFrame ef;
	ARP arp;
	PDU *pdu = &arp;
	ef.SetMACDATA(pdu);

	//EthernetFrame Header
	//由于类中的函数没做任何处理直接赋值，因此下面需要首先将主机字节序转换成网络字节序
	//inet_addr返回的已经是网络字节序，因此不必再转换
	ef.SetDestAddress(p->dstmac);
	ef.SetSourceAddress(p->srcmac);
	ef.SetEtherType(htons(ETHERTYPE_ARP));

	//ARP Header
	arp.setHardwareType(htons(HRD_ETHERNET));
	arp.setProtocolType(htons(PRO_IP));
	arp.setHardwareAddrLen(6);
	arp.setProtocolAddrLen(4);
	arp.setOpCode(htons(OP_REQUEST));
	arp.setSenderHardwareAddr(p->srcmac);
	arp.setSenderProtocolAddr(p->localip);
	arp.setTargetHardwareAddr(p->dstmac);

	BYTE buffer[60] = {0};
	GetLocalTime(&p->timesend);
	for(DWORD i=p->ipbeg; i<=p->ipend; ++i)
	{
		arp.setTargetProtocolAddr(htonl(i));
		ef.Write(buffer);
		p->device.sendPacket(buffer);
	}
	return 0;
}

UINT CLANScannerDlg::recvAll(LPVOID lpParam)
{
	CLANScannerDlg *p = (CLANScannerDlg *)lpParam;
	pcap_t *adhandle = p->device.getHandle();
	int res;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	EthernetFrame ef;
	ARP arp, *parp;
	PDU *pdu = &arp;
	ef.SetMACDATA(pdu);

    /* Retrieve the packets */
    while((res = pcap_next_ex( adhandle, &header, &pkt_data)) >= 0)
	{
        if(res == 0)
            /* Timeout elapsed */
            continue;
		
		ef.Read((BYTE *)pkt_data);
		parp = dynamic_cast<ARP *>(pdu);
		if(parp->getOpCode() == htons(OP_REPLY) && parp->getTargetProtocolAddr() == p->localip)
		{
			GetLocalTime(&p->timerecv);
			p->diff = 1000*(p->timerecv.wSecond-p->timesend.wSecond)+(p->timerecv.wMilliseconds-p->timesend.wMilliseconds);
			memcpy(p->hostmac, parp->getSenderHardwareAddr(), 6);
			p->hostip = parp->getSenderProtocolAddr();
			GlobalLock((HGLOBAL)p);
			AfxGetMainWnd()->SendMessage(WM_COMPLETE, (WPARAM)p, 0);
		}
    }
    
    if(res == -1){
        return -1;
    }
	return 0;
}

UINT CLANScannerDlg::hostOne(LPVOID lpParam)
{
	DWORD *ip= (DWORD *)lpParam;
	hostent *h = gethostbyaddr((char *)ip, 4, AF_INET);
	GlobalLock((HGLOBAL)ip);
	GlobalLock((HGLOBAL)h);
	if(h)
		AfxGetMainWnd()->SendMessage(WM_HOST, (WPARAM)h->h_name, (LPARAM)ip);
	return 0;
}

LRESULT CLANScannerDlg::OnComplete(WPARAM wParam, LPARAM lParam)
{
	CLANScannerDlg *p=(CLANScannerDlg *)wParam;
    int count = m_CListCtrl.GetItemCount();
	m_CListCtrl.InsertItem(count, CString(Common::dword2char(p->hostip)));
	m_CListCtrl.SetItemText(count, 1, CString(Common::mac2str(p->hostmac)));
	CString time;
	time.Format("%d ms", p->diff);
    m_CListCtrl.SetItemText(count, 3, time);
	hostip2 = p->hostip;
	GlobalLock((HGLOBAL)hostip2);
	CWinThread *hThreadhost = AfxBeginThread(hostOne, &hostip2, THREAD_PRIORITY_NORMAL, 0, 0, NULL);

	UpdateData(FALSE);
	GlobalUnlock((HGLOBAL)p);
	return 0;
}

LRESULT CLANScannerDlg::OnHost(WPARAM wParam, LPARAM lParam)
{
	char *h=(char *)wParam;
	DWORD *ip = (DWORD *)lParam;
	CString hstr(h);
	CString ipstr(Common::dword2char(*ip));

    int n = m_CListCtrl.GetItemCount();
    for (int i=0; i<n; ++i)
    {
        CString ipstr2 = m_CListCtrl.GetItemText(i, 0);
        if (ipstr == ipstr2)
            m_CListCtrl.SetItemText(i, 2, hstr);
    }

    GlobalUnlock((HGLOBAL)h);
	GlobalUnlock((HGLOBAL)ip);
	UpdateData(FALSE);

	return 0;
}

void CLANScannerDlg::OnBnClickedButtonScan()
{
	// TODO: 在此添加控件通知处理程序代码
	if(-1 == index)
		return;

	if (!started)
    {
        started = true;
        GetDlgItem(IDC_BUTTON_SCAN)->SetWindowText("&Stop");
        GetDlgItem(IDC_COMBO_NIC)->EnableWindow(FALSE);
        m_CListCtrl.DeleteAllItems();

		device.obtainDeviceList();
		device.openAdapter(nic.getNICname());
		string filter = "arp";
		device.setFilter(filter.c_str());

		memset(dstmac, 0xFF, 6);//broadcast
		memcpy(srcmac, nic.getMacAddress(), 6);
		localip = inet_addr(nic.getIPAddress());
		netmask = inet_addr(nic.getSubnetMask());
		ipbeg = ntohl(localip & netmask);//inet_addr返回的是网络字节序，需要转换成主机字节序进行ip扫描计算
		ipend = ipbeg + ~ntohl(netmask);

		hThreadsend = AfxBeginThread(sendAll, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		hThreadrecv = AfxBeginThread(recvAll, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
    }
    else
    {
        started = false;
        GetDlgItem(IDC_BUTTON_SCAN)->SetWindowText("&Scan");
        GetDlgItem(IDC_COMBO_NIC)->EnableWindow(TRUE);
        TerminateThread(hThreadsend, NULL);
		TerminateThread(hThreadrecv, NULL);
    }
}

void CLANScannerDlg::OnBnClickedButtonExit()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}

void CLANScannerDlg::OnBnClickedButtonAbout()
{
	// TODO: 在此添加控件通知处理程序代码
	MessageBox("Chenchao (113520081202014)\nEmail: orcish02@gmail.com\nSchool of Computing, CUC");
}