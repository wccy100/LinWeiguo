#pragma once

#include "Uri.h"
#include <string>

using namespace std;

class CRtsp
{
private:
	CUri m_uri;
	static string m_CSeq;
	string m_Session;
	void increment();
public:
	CRtsp();
	CRtsp(const char *uri);
	CRtsp(const CUri &cu);
	string getRequestCSeq() const;
	string getRequestSession() const;
	void setURI(const CUri &cu);
	void setRequestSession(const string &session);
	string createRequestOptions();
	string createRequestDescribe();
	string createRequestSetup(const string &clientPortRtp);
	string createRequestPlay();
	string createRequestPause();
	string createRequestTeardown();
	string createRequestGetParameter();
};