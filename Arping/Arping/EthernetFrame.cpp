#include "EthernetFrame.h"

EthernetFrame::EthernetFrame()
{
	memset(DestAddress, 0, 6);
	memset(SourceAddress, 0, 6);
	EtherType = 0;
	MACDATA = new ARP();
}

EthernetFrame::EthernetFrame(BYTE *m_DestAddress, BYTE *m_SourceAddress, ARP * DATA)
{
	memcpy(DestAddress, m_DestAddress, sizeof(DestAddress));
	memcpy(SourceAddress, m_SourceAddress, sizeof(SourceAddress));
	EtherType = htons(ETHERTYPE_ARP);
	MACDATA = DATA;
}

EthernetFrame::EthernetFrame(BYTE *buf)
{
	memcpy(DestAddress, buf, 6);
	memcpy(SourceAddress, buf+6, 6);
	memcpy(&EtherType, buf+12, 2);
	MACDATA = new ARP();
	memcpy(MACDATA,buf+14,28);
}

EthernetFrame::~EthernetFrame()
{
	delete MACDATA;
	MACDATA = NULL;
}

void EthernetFrame::frame2Byte(BYTE *buf)
{
	memcpy(buf, DestAddress, 6);
	memcpy(buf+6, SourceAddress, 6);
	memcpy(buf+12, &EtherType, 2);
	memcpy(buf+14, (BYTE *)MACDATA, 28);
	memset(buf+42, 0, 18);
}

void EthernetFrame::macdata2Arp(ARP &a)
{
	WORD hardware_type = 0;
	WORD protocol_type = 0;
	BYTE hardware_address_length = 0;
	BYTE protocol_address_length = 0;
	WORD operation_code = 0;
	BYTE source_hardware_address[6] = {0};
	DWORD source_protocol_address = 0;
	BYTE target_hardware_address[6] = {0};
	DWORD target_protocol_address = 0;

	memcpy(&hardware_type, MACDATA, 2);
	memcpy(&protocol_type, MACDATA+2, 2);
	memcpy(&hardware_address_length, MACDATA+4, 1);
	memcpy(&protocol_address_length, MACDATA+5, 1);
	memcpy(&operation_code, MACDATA+6, 2);
	memcpy(source_hardware_address, MACDATA+8, 6);
	memcpy(&source_protocol_address, MACDATA+14, 4);
	memcpy(target_hardware_address, MACDATA+18, 6);
	memcpy(&target_protocol_address, MACDATA+24, 4);

	a.setHardwareType(hardware_type);
	a.setProtocolType(protocol_type);
	a.setHardwareAddressLength(hardware_address_length);
	a.setProtocolAddressLength(protocol_address_length);
	a.setOperationCode(operation_code);
	a.setSourceHardwareAddress(source_hardware_address);
	a.setSourceProtocolAddress(source_protocol_address);
	a.setTargetHardwareAddress(target_hardware_address);
	a.setTargetProtocolAddress(target_protocol_address);
}

WORD EthernetFrame::getArpOperationCode()
{
	return MACDATA->getOperationCode();
}

BYTE *EthernetFrame::getArpSourceHardwareAddress()
{
	return MACDATA->getSourceHardwareAddress();
}

DWORD EthernetFrame::getArpSourceProtocolAddress()
{
	return MACDATA->getSourceProtocolAddress();
}