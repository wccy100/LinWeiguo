#pragma once

#include "Common.h"
#include "ARP.h"

static const WORD ETHERTYPE_ARP = 0x0806;

class EthernetFrame
{
private:
	BYTE DestAddress[6];
	BYTE SourceAddress[6];
	WORD EtherType;
	ARP * MACDATA;
public:
	EthernetFrame();
	EthernetFrame(BYTE *m_DestAddress, BYTE *m_SourceAddress, ARP * DATA);
	EthernetFrame(BYTE *buf);
	~EthernetFrame();
	void frame2Byte(BYTE *buf);
	void macdata2Arp(ARP &a);
	WORD getArpOperationCode();
	BYTE *getArpSourceHardwareAddress();
	DWORD getArpSourceProtocolAddress();
};