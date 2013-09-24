#pragma once

#ifndef HAVE_REMOTE
#define HAVE_REMOTE
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef WORD
typedef unsigned short WORD;
#endif

#ifndef DWORD
typedef unsigned long DWORD;
#endif

#include <iostream>
#include <string>
#include <winsock2.h>
#include "pcap.h"
#include <Windows.h>
#include <iphlpapi.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib,"iphlpapi.lib")

class Common
{
public:
	static void getLocalMac(BYTE *mac)
	{
        IP_ADAPTER_INFO adapter[5];  //Maximum 5 adapters
        DWORD buflen=sizeof(adapter);
        DWORD status=GetAdaptersInfo(adapter,&buflen);
        if(status==ERROR_SUCCESS)
        {
          PIP_ADAPTER_INFO painfo=adapter;
          memcpy(mac,painfo->Address,6);
        }
	}

	static char * dword2Char(const DWORD ip_dword)
	{
		in_addr ip_char;
		memcpy(&ip_char, &ip_dword, 4);
		return inet_ntoa(ip_char);
	}

	static float timeSub(const timeval &a, const timeval &b)
	{
		return (1000*(b.tv_sec - a.tv_sec)+(b.tv_usec - a.tv_usec)/(float)1000);
	}
};