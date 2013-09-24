#include "stdafx.h"
#include "Uri.h"

string CUri::hostPort()
{
	int beg = strlen("rtsp://");
	int end = m_Uri.find('/', beg);
	return m_Uri.substr(beg, end-beg);
}

CUri::CUri()
{

}

CUri::CUri(const string &uri)
{
	m_Uri = uri;
}

CUri::CUri(const char *uri)
{
	string s(uri);
	m_Uri = s;
}

string CUri::getUri() const
{
	return m_Uri;
}

void CUri::setUri(string &uri)
{
	m_Uri = uri;
}

string CUri::parseHost()
{
	string temp = hostPort();
	int pos = temp.find(':');
	if(pos != string::npos)
	{
		temp = temp.substr(0,pos);
	}

	if(isalpha(temp[0]))
	{
		string s(Common::Domain2IP(temp.c_str()));
		return s;
	}

	return temp;
}

string CUri::parsePort()
{
	string temp = hostPort();
	int pos = temp.find(':');
	if(pos == string::npos)
		return "554";
	else
	{
		return temp.substr(pos+1, temp.size()-pos);
	}
}

string CUri::parseFile()
{
	int first = m_Uri.find('/');
	first += 2;
	int second = m_Uri.find('/', first);
	++second;
	return m_Uri.substr(second, m_Uri.size()-second);
}