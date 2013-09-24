#pragma once

#include "Common.h"
#include "EthernetFrame.h"

class Device
{
private:
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int i;
	pcap_t *adhandle;
public:
	~Device();
	int obtainDeviceList();
	int openAdapter();
	int setFilter(const char *packet_filter);
	ULONG getIp();
	void sendPacket(const BYTE *packet);
	DWORD WINAPI recvPacket(LPVOID lpParam);
};