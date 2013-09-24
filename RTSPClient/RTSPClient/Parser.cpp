#include "stdafx.h"
#include "Parser.h"

void CParser::read(const char *buffer, int len)
{
	m_buffer.assign(buffer, len);
}

string CParser::parseResponseCSeq()
{
	int beg = m_buffer.find("CSeq");
	int end = m_buffer.find("\r\n", beg);
	return m_buffer.substr(beg+strlen("CSeq: "), end-beg-strlen("CSeq: "));
}

string CParser::parseResponseStatus()
{
	int beg = m_buffer.find(' ');
	int end = m_buffer.find(' ', beg+1);
	return m_buffer.substr(beg+1, end-beg-1);
}

string CParser::parseResponseSession()
{
	int beg = m_buffer.find("Session");
	int end = m_buffer.find("\r\n", beg);
	return m_buffer.substr(beg+strlen("Session: "), end-beg-strlen("Session: "));
}

string CParser::parseResponseTransportRTPServerPort()
{
	int beg = m_buffer.find("server_port");
	beg += strlen("server_port=");
	int end = m_buffer.find('-', beg);
	return m_buffer.substr(beg, end-beg);
}