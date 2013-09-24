#include "stdafx.h"
#include "Rtsp.h"

string CRtsp::m_CSeq = "1";

void CRtsp::increment()
{
	int i_CSeq = atoi(m_CSeq.c_str()) + 1;
	char c_CSeq[3];
	_itoa_s(i_CSeq, c_CSeq, 10);
	m_CSeq = string(c_CSeq);
}

CRtsp::CRtsp()
{
	
}

CRtsp::CRtsp(const char *uri)
{
	CUri cu(uri);
	m_uri = cu;
}

CRtsp::CRtsp(const CUri &cu)
{
	m_uri = cu;
}

string CRtsp::getRequestCSeq() const
{
	return m_CSeq;
}

string CRtsp::getRequestSession() const
{
	return m_Session;
}

void CRtsp::setURI(const CUri &cu)
{
	m_uri = cu;
}

void CRtsp::setRequestSession(const string &session)
{
	m_Session = session;
}

string CRtsp::createRequestOptions()
{
	increment();
	return "OPTIONS " + m_uri.getUri() + " RTSP/1.0\r\n" + 
		"CSeq: " + m_CSeq + "\r\n" + 
		"User-Agent: chenchao\r\n\r\n";
}

string CRtsp::createRequestDescribe()
{
	increment();
	return "DESCRIBE " + m_uri.getUri() + " RTSP/1.0\r\n" + 
		"CSeq: " + m_CSeq + "\r\n" + 
		"User-Agent: chenchao\r\n" + 
		"Accept: application/sdp\r\n\r\n";
}

string CRtsp::createRequestSetup(const string &clientPortRtp)
{
	increment();
	char temp[6] = {0};
	_itoa_s((atoi(clientPortRtp.c_str())+1), temp, 10);
	string clientPortRTCP(temp);
	return "SETUP " + m_uri.getUri() + " RTSP/1.0\r\n" + 
		"CSeq: " + m_CSeq + "\r\n" + 
		"User-Agent: chenchao\r\n" + 
		"Transport: RTP/AVP;unicast;client_port=" + clientPortRtp + "-" + clientPortRTCP + "\r\n\r\n";
}

string CRtsp::createRequestPlay()
{
	increment();
	return "PLAY " + m_uri.getUri() + " RTSP/1.0\r\n" + 
		"CSeq: " + m_CSeq + "\r\n" + 
		"User-Agent: chenchao\r\n" + 
		"Session: " + m_Session + "\r\n" + 
		"Range: npt=0.000-\r\n\r\n";
}

string CRtsp::createRequestPause()
{
	increment();
	return "PAUSE " + m_uri.getUri() + " RTSP/1.0\r\n" + 
		"CSeq: " + m_CSeq + "\r\n" + 
		"User-Agent: chenchao\r\n" + 
		"Session: " + m_Session + "\r\n\r\n";
}

string CRtsp::createRequestTeardown()
{
	increment();
	return "TEARDOWN " + m_uri.getUri() + " RTSP/1.0\r\n" + 
		"CSeq: " + m_CSeq + "\r\n" + 
		"User-Agent: chenchao\r\n" + 
		"Session: " + m_Session + "\r\n\r\n";
}

string CRtsp::createRequestGetParameter()
{
	increment();
	return "GET_PARAMETER " + m_uri.getUri() + " RTSP/1.0" + 
		"CSeq: " + m_CSeq + "\r\n" + 
		"User-Agent: chenchao\r\n" + 
		"Session: " + m_Session + "\r\n\r\n";
}