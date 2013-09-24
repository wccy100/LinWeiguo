
// PortScannerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PortScanner.h"
#include "PortScannerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPortScannerDlg dialog




CPortScannerDlg::CPortScannerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPortScannerDlg::IDD, pParent)
	, started(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPortScannerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO, m_CComboBox);
	DDX_Control(pDX, IDC_LIST, m_CListCtrl);
}

BEGIN_MESSAGE_MAP(CPortScannerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO, &CPortScannerDlg::OnCbnSelchangeCombo)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, &CPortScannerDlg::OnBnClickedButtonScan)
	ON_MESSAGE(WM_RECV,OnRecv)
END_MESSAGE_MAP()


// CPortScannerDlg message handlers

BOOL CPortScannerDlg::OnInitDialog()
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
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPortScannerDlg::OnPaint()
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
HCURSOR CPortScannerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPortScannerDlg::OnCbnSelchangeCombo()
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

void CPortScannerDlg::OnBnClickedButtonScan()
{
	// TODO: 在此添加控件通知处理程序代码
	if(-1 == index)
		return;

	if (!started)
    {
		count = 0;
        started = true;
        GetDlgItem(IDC_BUTTON_SCAN)->SetWindowText("&Stop");
        GetDlgItem(IDC_COMBO)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STARTINGPORT)->GetWindowText(m_startingport);
		GetDlgItem(IDC_EDIT_ENDINGPORT)->GetWindowText(m_endingport);
		GetDlgItem(IDC_EDIT_IPADDRESS)->GetWindowText(m_ipaddress);
		checked = IsDlgButtonChecked(IDC_CHECK);
        m_CListCtrl.DeleteAllItems();
		m_CListCtrl.InsertItem(count++,"Starting scan");

		m_sport = atoi(m_startingport);
		m_eport = atoi(m_endingport);
		string ipstring(m_ipaddress.GetBuffer());
		m_ipaddress.ReleaseBuffer();
		m_dstip = inet_addr(ipstring.c_str());

		if((m_dstip&m_mask) == (m_srcip&m_mask))
			m_dstmac = nic.getLanMac(m_dstip);
		else
			m_dstmac = m_gwmac;

		device.obtainDeviceList();
		device.openAdapter(nic.getNICname());
		string filter = "tcp";
		device.setFilter(filter.c_str());

		hThreadrecv = AfxBeginThread(recv, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		hThreadsend = AfxBeginThread(send, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	}
	else
    {
        started = false;
        GetDlgItem(IDC_BUTTON_SCAN)->SetWindowText("&Scan");
        GetDlgItem(IDC_COMBO)->EnableWindow(TRUE);
		TerminateThread(hThreadsend, NULL);
		TerminateThread(hThreadrecv, NULL);
		m_CListCtrl.InsertItem(count,"Finished scan");
    }
}

UINT CPortScannerDlg::recv(LPVOID lpParam)
{
	CPortScannerDlg *p = (CPortScannerDlg *)lpParam;
	pcap_t *adhandle = p->device.getHandle();
	int res;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	EthernetFrame ef;
	TCP tcp, *ptcp;
	PDU *pdu = &tcp;
	ef.SetMACDATA(pdu);

	WORD dp;
	BYTE flags;
    /* Retrieve the packets */
    while((res = pcap_next_ex( adhandle, &header, &pkt_data)) >= 0)
	{
        if(res == 0)
            /* Timeout elapsed */
            continue;

		ef.Read((BYTE *)pkt_data);
		ptcp = dynamic_cast<TCP *>(pdu);//dynamic_cast运算符可以在执行期决定真正的类型。
										//如果downcast是安全的（也就说，如果基类指针或者引用确实指向一个派生类对象）这个运算符会传回适当转型过的指针。
										//如果downcast不安全，这个运算符会传回空指针（也就是说，基类指针或者引用没有指向一个派生类对象）。
		dp = ptcp->getDestinationport();
		flags = ptcp->getUnusedFlags();
		if (dp == htons(0xabcd) && flags == 0x12)
		{
			p->sp = ptcp->getSourceport();
			GlobalLock((HGLOBAL)p);
			AfxGetMainWnd()->SendMessage(WM_RECV, (WPARAM)p, 0);
		}
    }
    
    if(res == -1){
        return -1;
    }
	return 0;
}

LRESULT CPortScannerDlg::OnRecv(WPARAM wParam, LPARAM lParam)
{
	CPortScannerDlg *p = (CPortScannerDlg *)wParam;
	CString ps;
	ps.Format("%s:%d Connection accepted", Common::dword2char(p->m_dstip), ntohs(p->sp));
	m_CListCtrl.InsertItem(count++, ps);

	UpdateData(FALSE);
	GlobalUnlock((HGLOBAL)p);
	return 0;
}

UINT CPortScannerDlg::send(LPVOID lpParam)
{
	CPortScannerDlg *p = (CPortScannerDlg *)lpParam;
	EthernetFrame ef;
	TCP tcp;
	PDU *pdu = &tcp;
	ef.SetMACDATA(pdu);

	BYTE buffer[54] = {0};
	for(WORD i=p->m_sport; i<=p->m_eport; ++i)
	{
		//ethernetframe header
		ef.SetDestAddress(p->m_dstmac);
		ef.SetSourceAddress(p->m_srcmac);
		ef.SetEtherType(htons(ETHERTYPE_IPV4));

		//ip header
		tcp.setVersionIHL(0x45);
		tcp.setDifferentiatedServices(0);
		tcp.setTotallength(htons(40));
		tcp.setIdentification(htons(1));
		tcp.setUnusedDFMFFragmentoffset(0);
		tcp.setTimetolive(16);
		tcp.setProtocol(6);
		tcp.setHeaderchecksum(0);
		tcp.setSourceaddress(p->m_srcip);
		tcp.setDestinationaddress(p->m_dstip);

		//tcp header
		tcp.setSourceport(htons(0xabcd));
		tcp.setDestinationport(htons(i));
		tcp.setSequencenumber(htonl(1));
		tcp.setAcknowledgementnumber(0);
		tcp.setTCPheaderlengthUnused(0x50);
		tcp.setUnusedFlags(0x02);
		tcp.setWindowsize(htons(0xffff));
		tcp.setChecksum(0);
		tcp.setUrgentpointer(0);

		//ip checksum recalculate
		ef.Write(buffer);
		WORD ipchecksum = Common::CalculateCheckSum(buffer+14, 20);
		tcp.setHeaderchecksum(ipchecksum);
		ef.Write(buffer);

		//tcp fake header
		BYTE temp[32];
		DWORD srcip = tcp.getSourceaddress();
		DWORD dstip = tcp.getDestinationaddress();
		memcpy(temp, &srcip, 4);
		memcpy(temp+4, &dstip, 4);
		temp[8] = 0;
		temp[9] = 6;
		WORD TCPsegmentlength = htons(20);
		memcpy(temp+10, &TCPsegmentlength, 2);
		memcpy(temp+12, buffer+34, 20);
		
		//tcp checksum recalculate
		WORD Checksum = Common::CalculateCheckSum(temp, 32);
		tcp.setChecksum(Checksum);
		ef.Write(buffer);

		//send tcp half open request
		p->device.sendPacket(buffer);
	}

	return 0;
}