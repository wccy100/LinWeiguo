#pragma once

#include "Common.h"
#include <string>

using namespace std;

class CUri
{
private:
	string m_Uri;
	string hostPort();
public:
	CUri();
	CUri(const string &uri);
	CUri(const char *uri);
	string getUri() const;
	void setUri(string &uri);
	string parseHost();
	string parsePort();
	string parseFile();
};