
// RTSPClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RTSPClient.h"
#include "RTSPClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const string CRTSPClientDlg::CLIENT_PORT_RTP = "1554";

// CRTSPClientDlg �Ի���




CRTSPClientDlg::CRTSPClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRTSPClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRTSPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_URI, m_CEdit_URI);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD, m_CButton_Download);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_CButton_Play);
	DDX_Control(pDX, IDC_STATIC_VLC, m_CStatic_VLC);
}

BEGIN_MESSAGE_MAP(CRTSPClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CRTSPClientDlg::OnBnClickedButtonDownload)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CRTSPClientDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CRTSPClientDlg::OnBnClickedButtonPlay)
END_MESSAGE_MAP()


// CRTSPClientDlg ��Ϣ�������

BOOL CRTSPClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_DownloadClicked = false;
	m_Downloading = false;
	m_Teardowned = false;
	m_Playing = false;
	if(CBlockingSocket::Initialize())
	{
		AfxMessageBox(_T("WSAStartup failed."));
		return FALSE;
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRTSPClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CRTSPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CRTSPClientDlg::RTSPRequest(int method)
{
	char rtspBuf[RTSP_BUF] = {0};
	int rtspLen = 0;
	string request, cseq, status, session;

	switch(method)
	{
	case 2:
		request = m_CRtsp.createRequestOptions();
		break;
	case 3:
		request = m_CRtsp.createRequestDescribe();
		break;
	case 4:
		request = m_CRtsp.createRequestSetup(CLIENT_PORT_RTP);
		break;
	case 5:
		request = m_CRtsp.createRequestPlay();
		break;
	case 6:
		request = m_CRtsp.createRequestPause();
		break;
	case 7:
		request = m_CRtsp.createRequestTeardown();
		break;
	case 8:
		request = m_CRtsp.createRequestGetParameter();
		break;
	default:
		break;
	}
	
	m_RTSPSocket.Send(request.c_str(), request.size());
	rtspLen = m_RTSPSocket.Recv(rtspBuf, sizeof(rtspBuf));
	m_CParser.read(rtspBuf, rtspLen);
	cseq = m_CParser.parseResponseCSeq();
	status = m_CParser.parseResponseStatus();

	switch(method)
	{
	case 2:
		if(cseq == m_CRtsp.getRequestCSeq() && status == "200")
			return 0;
		break;
	case 3:
		if(cseq == m_CRtsp.getRequestCSeq() && status == "200")
			return 0;
		break;
	case 4:
		session = m_CParser.parseResponseSession();
		m_CRtsp.setRequestSession(session);

		if(cseq == m_CRtsp.getRequestCSeq() && status == "200")
			return 0;
		break;
	case 5:
		session = m_CParser.parseResponseSession();

		if(cseq == m_CRtsp.getRequestCSeq() && status == "200" && session == m_CRtsp.getRequestSession())
			return 0;
		break;
	case 6:
		session = m_CParser.parseResponseSession();

		if(cseq == m_CRtsp.getRequestCSeq() && status == "200" && session == m_CRtsp.getRequestSession())
			return 0;
		break;
	case 7:
		if(cseq == m_CRtsp.getRequestCSeq() && status == "200")
			return 0;
		break;
	case 8:
		if(cseq == m_CRtsp.getRequestCSeq() && status == "200")
			return 0;
		break;
	default:
		break;
	}

	return 1;
}

//
// This function checks if there was a RTP error. If so, it displays an error
// message and exists.
//

void CRTSPClientDlg::checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		CString err;
		string temp = "ERROR: " + RTPGetErrorString(rtperr);
		err = temp.c_str();
		AfxMessageBox(err);
		exit(-1);
	}
}

UINT CRTSPClientDlg::RTSPThreadProc(LPVOID pParam)
{
	CRTSPClientDlg *p = (CRTSPClientDlg *)pParam;

	if(p->m_RTSPSocket.Open(TCP))
	{
		AfxMessageBox(_T("socket failed."));
		return 1;
	}
	if(p->m_RTSPSocket.Connect(p->m_RTSPServerIP.c_str(), p->m_RTSPServerPort.c_str()))
	{
		AfxMessageBox(_T("connect failed."));
		return 1;
	}

	p->m_CRtsp.setURI(p->m_CUri);

	if(p->RTSPRequest(OPTIONS))
	{
		AfxMessageBox(_T("OPTIONS failed."));
		return 1;
	}
	if(p->RTSPRequest(DESCRIBE))
	{
		AfxMessageBox(_T("DESCRIBE failed."));
		return 1;
	}
	if(p->RTSPRequest(SETUP))
	{
		AfxMessageBox(_T("SETUP failed."));
		return 1;
	}
	if(p->RTSPRequest(PLAY))
	{
		AfxMessageBox(_T("PLAY failed."));
		return 1;
	}
	while(1)
	{
		//ʹ��WaitForSingleObject�ȴ��¼��ں˶��󱻴�����һ��������������
		//ֵ����WAIT_OBJECT_0�������߳�ͨ��return 0ֱ�ӷ���
		if(WaitForSingleObject(p->m_hEventRTSP,0) == WAIT_OBJECT_0)
		{
			DWORD dwExitCode = GetExitCodeThread(p->m_pThreadRTSP, &dwExitCode);
			CloseHandle(p->m_hEventRTSP);
			AfxEndThread(dwExitCode, TRUE);
			return 0;
		}
		Sleep(60000);
		if(p->RTSPRequest(GET_PARAMETER))
		{
			AfxMessageBox(_T("GET_PARAMETER failed."));
			return 1;
		}
	}

	return 0;
}

UINT CRTSPClientDlg::RTPThreadProc(LPVOID pParam)
{
	CRTSPClientDlg *p = (CRTSPClientDlg *)pParam;

	RTPSession sess;
	uint16_t portbase = atoi(p->CLIENT_PORT_RTP.c_str());
	std::string ipstr;
	int status,num = 60;
	
	// Now, we'll create a RTP session, set the destination
	// and poll for incoming data.
	
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;
	
	// IMPORTANT: The local timestamp unit MUST be set, otherwise
	//            RTCP Sender Report info will be calculated wrong
	// In this case, we'll be just use 8000 samples per second.
	sessparams.SetOwnTimestampUnit(1.0/8000.0);		
	
	transparams.SetPortbase(portbase);
	status = sess.Create(sessparams,&transparams);	
	checkerror(status);

	fstream fs;
	fs.open(p->m_filename.c_str(), std::ios::out|std::ios::binary);

	while(1)
	{
		//ʹ��WaitForSingleObject�ȴ��¼��ں˶��󱻴�����һ��������������
		//ֵ����WAIT_OBJECT_0�������߳�ͨ��return 0ֱ�ӷ���
		if(WaitForSingleObject(p->m_hEventRTP,0) == WAIT_OBJECT_0)
		{
			fs.close();
			sess.BYEDestroy(RTPTime(1,0),0,0);
			DWORD dwExitCode = GetExitCodeThread(p->m_pThreadRTP, &dwExitCode);
			CloseHandle(p->m_hEventRTP);
			AfxEndThread(dwExitCode, TRUE);
			return 0;
		}

		sess.BeginDataAccess();
		
		// check incoming packets
		if (sess.GotoFirstSourceWithData())
		{
			do
			{
				RTPPacket *pack;
				
				while ((pack = sess.GetNextPacket()) != NULL)
				{
					// You can examine the data here
					uint8_t *mp3 = pack->GetPayloadData();
					size_t mp3len = pack->GetPayloadLength();
					fs.write((char *)mp3+4,mp3len-4);
					
					// we don't longer need the packet, so
					// we'll delete it
					sess.DeletePacket(pack);
				}
			} while (sess.GotoNextSourceWithData());
		}
		
		sess.EndDataAccess();

#ifndef RTP_SUPPORT_THREAD
		status = sess.Poll();
		checkerror(status);
#endif // RTP_SUPPORT_THREAD

	}

	fs.close();
	
	sess.BYEDestroy(RTPTime(1,0),0,0);

	return 0;
}

void CRTSPClientDlg::OnBnClickedButtonDownload()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_DownloadClicked = true;
	if(!m_Downloading)
	{
		CString cstrURI;
		m_CEdit_URI.GetWindowTextW(cstrURI);
		if(Common::ValidateURI(cstrURI) == 0)
		{
			AfxMessageBox(_T("URI is invalid."));
			return;
		}

		m_Downloading = true;
		m_CButton_Download.SetWindowTextW(_T("Stop"));
		m_CEdit_URI.SetReadOnly();

		m_hEventRTP = CreateEvent(NULL,TRUE,FALSE,NULL);//�ֶ���������ʼδ����״̬
		m_pThreadRTP = AfxBeginThread(RTPThreadProc, this);
		if(m_pThreadRTP == NULL)
		{
			AfxMessageBox(_T("RTP thread failed."));
			return;
		}

		USES_CONVERSION;
		string strURI = W2A(cstrURI.GetBuffer());
		cstrURI.ReleaseBuffer();

		m_CUri.setUri(strURI);
		m_RTSPServerIP = m_CUri.parseHost();
		m_RTSPServerPort = m_CUri.parsePort();
		m_filename = m_CUri.parseFile();
		
		m_hEventRTSP = CreateEvent(NULL,TRUE,FALSE,NULL);//�ֶ���������ʼδ����״̬
		m_pThreadRTSP = AfxBeginThread(RTSPThreadProc, this);
		if(m_pThreadRTSP == NULL)
		{
			AfxMessageBox(_T("RTSP thread failed."));
			return;
		}
	}
	else
	{
		SetEvent(m_hEventRTP);//�����¼��ں˶���
		SetEvent(m_hEventRTSP);//�����¼��ں˶���
		m_Downloading = false;
		m_CButton_Download.SetWindowTextW(_T("Download"));
		m_CEdit_URI.SetReadOnly(0);
		if(RTSPRequest(TEARDOWN))
		{
			AfxMessageBox(_T("TEARDOWN failed."));
			return;
		}
		m_Teardowned = true;
		m_RTSPSocket.Close();
		m_pThreadRTSP = NULL;
		m_pThreadRTP = NULL;
		libvlc_media_player_release (mp);
		libvlc_release (inst);
	}
}

void CRTSPClientDlg::OnBnClickedButtonClose()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetEvent(m_hEventRTP);//�����¼��ں˶���
	SetEvent(m_hEventRTSP);//�����¼��ں˶���
	if(!m_Teardowned && m_DownloadClicked)
		if(RTSPRequest(TEARDOWN))
			AfxMessageBox(_T("TEARDOWN failed."));
	Sleep(100);
	CBlockingSocket::Cleanup();
	CDialog::OnCancel();
}

void CRTSPClientDlg::OnBnClickedButtonPlay()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(!m_Playing)
	{
		fstream fs;
		fs.open(m_filename.c_str(), ios::in|ios::binary);
		if(!fs)
		{
			fs.close();
			AfxMessageBox(_T("Download first."));
			return;
		}

		m_Playing = true;
		m_CButton_Play.SetWindowTextW(_T("Stop"));

		int vlc_argc = 0; 
		char *vlc_argv[100]; 
		vlc_argv[vlc_argc++] = "--ignore-config"; 
		inst = libvlc_new(vlc_argc, vlc_argv); 
		m = libvlc_media_new_path(inst, m_filename.c_str()); 
		mp = libvlc_media_player_new_from_media(m);
		libvlc_media_release (m);
		void *hwnd =(void *) m_CStatic_VLC.GetSafeHwnd(); 
		libvlc_media_player_set_hwnd(mp, hwnd); 
		libvlc_media_player_play(mp);
	}
	else
	{
		m_Playing = false;
		m_CButton_Play.SetWindowTextW(_T("Play"));

		libvlc_media_player_release (mp);
		libvlc_release (inst);
	}
}
