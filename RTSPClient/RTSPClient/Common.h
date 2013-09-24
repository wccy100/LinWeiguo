#pragma once

#include <winsock2.h>
#include <stdio.h>
#include <afxinet.h>

#pragma comment(lib, "Ws2_32.lib")

class Common
{
public:
	static char *GetLocalIP();
	static char *Domain2IP(const char *domain);
	static BOOL ValidateURI(CString &cstrURI);
};