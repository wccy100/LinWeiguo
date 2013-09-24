
// RTSPClientDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "Common.h"
#include "BlockingSocket.h"
#include "Uri.h"
#include "Rtsp.h"
#include "Parser.h"

#include "rtpsession.h"
#include "rtppacket.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include <winsock2.h>
#include "rtpsourcedata.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "vlc/vlc.h"

#pragma comment(lib, "jrtplib_d.lib")
#pragma comment(lib,"jthread_d.lib")
#pragma comment(lib,"WS2_32.lib")
#pragma comment(lib, "libvlc.lib")
#pragma comment(lib, "libvlccore.lib")

using namespace jrtplib;


// CRTSPClientDlg 对话框
class CRTSPClientDlg : public CDialog
{
// 构造
public:
	CRTSPClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_RTSPCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	static const int BDP = 32768;
	static const int RTSP_BUF = 1024;
	static const int TCP = 0;
	static const int UDP = 1;
	static const int OPTIONS = 2;
	static const int DESCRIBE = 3;
	static const int SETUP = 4;
	static const int PLAY = 5;
	static const int PAUSE = 6;
	static const int TEARDOWN = 7;
	static const int GET_PARAMETER = 8;
	static const string CLIENT_PORT_RTP;

	CEdit m_CEdit_URI;
	CButton m_CButton_Download;
	CButton m_CButton_Play;
	CStatic m_CStatic_VLC;

	CUri m_CUri;
	CBlockingSocket m_RTSPSocket;
	CRtsp m_CRtsp;
	CParser m_CParser;
	string m_RTSPServerIP;
	string m_RTSPServerPort;
	string m_filename;
	bool m_Downloading;
	bool m_DownloadClicked;
	bool m_Teardowned;
	bool m_Playing;
	CWinThread *m_pThreadRTSP;
	CWinThread *m_pThreadRTP;
	HANDLE m_hEventRTP;
	HANDLE m_hEventRTSP;
	libvlc_instance_t *inst;
	libvlc_media_t *m;
	libvlc_media_player_t *mp;

	int RTSPRequest(int method);
	static void checkerror(int rtperr);
	static UINT RTSPThreadProc(LPVOID pParam);
	static UINT RTPThreadProc(LPVOID pParam);
	afx_msg void OnBnClickedButtonDownload();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonPlay();
};
