#pragma once

#include "Common.h"

static const WORD HARDTYPE_ETHER = 0x0001;

static const WORD PROTOTYPE_IPV4 = 0x0800;

static const WORD ARP_REQUEST = 0x0001;
static const WORD ARP_REPLY = 0x0002;

#pragma pack(1)
class ARP
{
private:
	WORD hardware_type;
	WORD protocol_type;
	BYTE hardware_address_length;
	BYTE protocol_address_length;
	WORD operation_code;
	BYTE source_hardware_address[6];
	DWORD source_protocol_address;
	BYTE target_hardware_address[6];
	DWORD target_protocol_address;
public:
	ARP();
	ARP(BYTE *sha, DWORD spa, BYTE *tha, DWORD tpa);
	void setHardwareType(WORD h_t);
	void setProtocolType(WORD p_t);
	void setHardwareAddressLength(BYTE h_a_l);
	void setProtocolAddressLength(BYTE p_a_l);
	void setOperationCode(WORD o_c);
	void setSourceHardwareAddress(BYTE *s_h_a);
	void setSourceProtocolAddress(DWORD s_p_a);
	void setTargetHardwareAddress(BYTE *t_h_a);
	void setTargetProtocolAddress(DWORD t_p_a);
	WORD getOperationCode();
	BYTE *getSourceHardwareAddress();
	DWORD getSourceProtocolAddress();
};
#pragma pack()