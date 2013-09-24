#include "stdafx.h"
#include "Common.h"

char *Common::GetLocalIP()
{
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return NULL;
	}

	char hostname[255];
    if(gethostname(hostname, sizeof(hostname)) != 0)
    {
        printf("gethostname Error:%d\n", WSAGetLastError());
        return NULL;
    }

	hostent * hostinfo;
    if((hostinfo = gethostbyname(hostname)) == NULL)
    {
        printf("gethostbyname Error:%d\n", WSAGetLastError());
        return NULL;
    }

    char *ip;
    while(*(hostinfo->h_addr_list) != NULL)
    {
        ip = inet_ntoa(*(struct in_addr *) *hostinfo->h_addr_list);
		return ip;
        hostinfo->h_addr_list++;
    }

    WSACleanup();
	return NULL;
}

char *Common::Domain2IP(const char *domain)
{
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return NULL;
	}

	hostent *remoteHost = gethostbyname(domain);
	if(remoteHost != NULL)
	{
		int i = 0;
		in_addr addr;
		while(remoteHost->h_addr_list[i] != 0)
		{
			addr.S_un.S_addr = *(u_long *)remoteHost->h_addr_list[i++];
			return inet_ntoa(addr);
		}
	}

	WSACleanup();
	return NULL;
}

BOOL Common::ValidateURI(CString &cstrURI)
{
	cstrURI.TrimLeft();
	cstrURI.TrimRight();

	DWORD dwServiceType;
    CString strServer;
	CString strObject;
	INTERNET_PORT nPort;
	return AfxParseURL(cstrURI, dwServiceType, strServer, strObject, nPort);
}