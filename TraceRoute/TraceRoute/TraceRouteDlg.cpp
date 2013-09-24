
// TraceRouteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TraceRoute.h"
#include "TraceRouteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTraceRouteDlg dialog




CTraceRouteDlg::CTraceRouteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTraceRouteDlg::IDD, pParent)
	, started(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTraceRouteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO, m_CComboBox);
	DDX_Control(pDX, IDC_LIST, m_CListCtrl);
	DDX_Control(pDX, IDC_IPADDRESS, m_CIPAddressCtrl);
}

BEGIN_MESSAGE_MAP(CTraceRouteDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CTraceRouteDlg::OnBnClickedButtonAbout)
	ON_BN_CLICKED(IDC_BUTTON_TRACE, &CTraceRouteDlg::OnBnClickedButtonTrace)
	ON_CBN_SELCHANGE(IDC_COMBO, &CTraceRouteDlg::OnCbnSelchangeCombo)
	ON_MESSAGE(WM_RECV,OnRecv)
	ON_MESSAGE(WM_HOST,OnHost)
END_MESSAGE_MAP()


// CTraceRouteDlg message handlers

BOOL CTraceRouteDlg::OnInitDialog()
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
	m_CListCtrl.InsertColumn(0, _T("Hop"), LVCFMT_LEFT, 65, -1);
    m_CListCtrl.InsertColumn(1, _T("Time(ms)"), LVCFMT_LEFT, 90, -1);
    m_CListCtrl.InsertColumn(2, _T("IP Address"), LVCFMT_LEFT, 105, -1);
    m_CListCtrl.InsertColumn(3, _T("RouterName"), LVCFMT_LEFT, 120, -1);
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTraceRouteDlg::OnPaint()
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
HCURSOR CTraceRouteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTraceRouteDlg::OnBnClickedButtonAbout()
{
	// TODO: 在此添加控件通知处理程序代码
	MessageBox("Chenchao (113520081202014)\nEmail: orcish02@gmail.com\nSchool of Computing, CUC");
}

UINT CTraceRouteDlg::hostOne(LPVOID lpParam)
{
	DWORD *ip= (DWORD *)lpParam;
	hostent *h = gethostbyaddr((char *)ip, 4, AF_INET);
	GlobalLock((HGLOBAL)ip);
	GlobalLock((HGLOBAL)h);
	if(h)
		AfxGetMainWnd()->SendMessage(WM_HOST, (WPARAM)h->h_name, (LPARAM)ip);
	return 0;
}

UINT CTraceRouteDlg::recvsend(LPVOID lpParam)
{
	CTraceRouteDlg *p = (CTraceRouteDlg *)lpParam;
	pcap_t *adhandle = p->device.getHandle();
	int res;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	EthernetFrame ef;
	ICMP icmp, *picmp;
	PDU *pdu = &icmp;
	ef.SetMACDATA(pdu);

	BYTE buffer[106];
	WORD id;
    /* Retrieve the packets */
    while((res = pcap_next_ex( adhandle, &header, &pkt_data)) >= 0)
	{
        if(res == 0)
            /* Timeout elapsed */
            continue;

		ef.Read((BYTE *)pkt_data);
		picmp = dynamic_cast<ICMP *>(pdu);
		
		if(picmp->getType() != 0x8)
		{
			switch (picmp->getType())
			{
			case 0x0:
				if(picmp->getId() == htons(0xabcd))
				{
					GetLocalTime(&p->timerecv);
					p->timesub = 1000*(p->timerecv.wSecond-p->timesend.wSecond)+(p->timerecv.wMilliseconds-p->timesend.wMilliseconds);
					p->routerip = picmp->getSourceaddress();
					GlobalUnlock((HGLOBAL)p);
					AfxGetMainWnd()->SendMessage(WM_RECV, 0, 0);
					CWinThread *hThreadhost = AfxBeginThread(hostOne, &p->routerip, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
					return 0;
				}
				break;
			case 0xb:
				memcpy(&id, pkt_data+66, 2);
				if(id == htons(0xabcd))
				{
					GetLocalTime(&p->timerecv);
					p->timesub = 1000*(p->timerecv.wSecond-p->timesend.wSecond)+(p->timerecv.wMilliseconds-p->timesend.wMilliseconds);
					p->routerip = picmp->getSourceaddress();
					GlobalUnlock((HGLOBAL)p);
					AfxGetMainWnd()->SendMessage(WM_RECV, 0, 0);
					p->icmpRequest(buffer, ++p->ttl);
					p->device.sendPacket(buffer);
					GetLocalTime(&p->timesend);

					CWinThread *hThreadhost = AfxBeginThread(hostOne, &p->routerip, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
				}
				break;
			default:
				break;
			}
		}
    }
    
    if(res == -1){
        return -1;
    }
	return 0;
}

LRESULT CTraceRouteDlg::OnRecv(WPARAM wParam, LPARAM lParam)
{
    CString ltt;
    ltt.Format("%d", ttl);
	CString time;
	time.Format("%d", timesub);
	CString picrs(Common::dword2char(routerip));
	int count = m_CListCtrl.GetItemCount();
	m_CListCtrl.InsertItem(count, ltt);
	m_CListCtrl.SetItemText(count, 1, time);
    m_CListCtrl.SetItemText(count, 2, picrs);

	UpdateData(FALSE);
	GlobalUnlock((HGLOBAL)this);
	return 0;
}

LRESULT CTraceRouteDlg::OnHost(WPARAM wParam, LPARAM lParam)
{
	char *h=(char *)wParam;
	DWORD * ip = (DWORD *)lParam;
	CString hstr(h);
	CString ipstr(Common::dword2char(*ip));

    int n = m_CListCtrl.GetItemCount();
    for (int i=0; i<n; ++i)
    {
        CString ipstr2 = m_CListCtrl.GetItemText(i, 2);
        if (ipstr == ipstr2)
            m_CListCtrl.SetItemText(i, 3, hstr);
    }

    GlobalUnlock((HGLOBAL)h);
	GlobalUnlock((HGLOBAL)ip);
	UpdateData(FALSE);

	return 0;
}

void CTraceRouteDlg::OnBnClickedButtonTrace()
{
	// TODO: 在此添加控件通知处理程序代码
	if(-1 == index)
		return;

	if (!started)
    {
        started = true;
        GetDlgItem(IDC_BUTTON_TRACE)->SetWindowText("&Stop");
        GetDlgItem(IDC_COMBO)->EnableWindow(FALSE);
        m_CListCtrl.DeleteAllItems();

		BYTE field[4];
		m_CIPAddressCtrl.GetAddress(field[0], field[1], field[2], field[3]);
		CString dstip;
		dstip.Format("%d.%d.%d.%d",field[0],field[1],field[2],field[3]);
		m_dstip = inet_addr(dstip.GetBuffer());
		dstip.ReleaseBuffer();

		if((m_dstip&m_mask) == (m_srcip&m_mask))
			m_dstmac = nic.getLanMac(m_dstip);
		else
			m_dstmac = m_gwmac;

		device.obtainDeviceList();
		device.openAdapter(nic.getNICname());
		string filter = "icmp";
		device.setFilter(filter.c_str());

		hThread = AfxBeginThread(recvsend, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);

		BYTE buffer[106];
		ttl = 1;
		icmpRequest(buffer, ttl);
		device.sendPacket(buffer);
		GetLocalTime(&timesend);
	}
	else
    {
        started = false;
        GetDlgItem(IDC_BUTTON_TRACE)->SetWindowText("&Trace");
        GetDlgItem(IDC_COMBO)->EnableWindow(TRUE);
		TerminateThread(hThread, NULL);
    }
}

void CTraceRouteDlg::icmpRequest(BYTE *buffer, BYTE ttl)
{
	EthernetFrame ef;
	ICMP icmp;
	PDU *pdu = &icmp;
	ef.SetMACDATA(pdu);

	//ethernetframe header
	ef.SetDestAddress(m_dstmac);
	ef.SetSourceAddress(m_srcmac);
	ef.SetEtherType(htons(ETHERTYPE_IPV4));

	//ip header
	icmp.setVersionIHL(0x45);
	icmp.setDifferentiatedServices(0);
	icmp.setTotallength(htons(92));
	icmp.setIdentification(htons(0xabcd));
	icmp.setUnusedDFMFFragmentoffset(0);
	icmp.setTimetolive(ttl);
	icmp.setProtocol(1);
	icmp.setHeaderchecksum(0);
	icmp.setSourceaddress(m_srcip);
	icmp.setDestinationaddress(m_dstip);

	//icmp header
	icmp.setType(8);
	icmp.setCode(0);
	icmp.setChecksum(0);
	icmp.setId(htons(0xabcd));
	icmp.setSeq(htons(0xabcd));
	BYTE data[64] = {0};
	icmp.setData(data);

	//ip checksum recalculate
	ef.Write(buffer);
	WORD ipchecksum = Common::CalculateCheckSum(buffer+14, 20);
	icmp.setHeaderchecksum(ipchecksum);

	//icmp checksum recalculate
	WORD icmpchecksum = Common::CalculateCheckSum(buffer+34, 8);
	icmp.setChecksum(icmpchecksum);

	ef.Write(buffer);
}

void CTraceRouteDlg::OnCbnSelchangeCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	index = m_CComboBox.GetCurSel();
	nic.selectNIC(index);
	m_srcip = inet_addr(nic.getIPAddress());
	m_srcmac = nic.getMacAddress();
	m_mask = inet_addr(nic.getSubnetMask());
	m_gwmac = nic.getGatewayMac();

	UpdateData(FALSE);
}
