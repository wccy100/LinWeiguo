#pragma once

#include <string>

using namespace std;

class CParser
{
private:
	string m_buffer;
public:
	void read(const char *buffer, int len);
	string parseResponseCSeq();
	string parseResponseStatus();
	string parseResponseSession();
	string parseResponseTransportRTPServerPort();
};