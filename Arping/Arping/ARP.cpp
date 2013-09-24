#include "ARP.h"

ARP::ARP()
{
	hardware_type = 0;
	protocol_type = 0;
	hardware_address_length = 0;
	protocol_address_length = 0;
	operation_code = 0;
	memset(source_hardware_address, 0, 6);
	source_protocol_address = 0;
	memset(target_hardware_address, 0, 6);
	target_protocol_address = 0;
}

ARP::ARP(BYTE *sha, DWORD spa, BYTE *tha, DWORD tpa)
{
	hardware_type = htons(HARDTYPE_ETHER);
	protocol_type = htons(PROTOTYPE_IPV4);
	hardware_address_length = 0x6;
	protocol_address_length = 0x4;
	operation_code = htons(ARP_REQUEST);
	memcpy(source_hardware_address, sha, 6);
	source_protocol_address = spa;
	memcpy(target_hardware_address, tha, 6);
	target_protocol_address = tpa;
}

void ARP::setHardwareType(WORD h_t)
{
	hardware_type = h_t;
}

void ARP::setProtocolType(WORD p_t)
{
	protocol_type = p_t;
}

void ARP::setHardwareAddressLength(BYTE h_a_l)
{
	hardware_address_length = h_a_l;
}

void ARP::setProtocolAddressLength(BYTE p_a_l)
{
	protocol_address_length = p_a_l;
}

void ARP::setOperationCode(WORD o_c)
{
	operation_code = o_c;
}

void ARP::setSourceHardwareAddress(BYTE *s_h_a)
{
	memcpy(source_hardware_address, s_h_a, 6);
}

void ARP::setSourceProtocolAddress(DWORD s_p_a)
{
	source_protocol_address = s_p_a;
}

void ARP::setTargetHardwareAddress(BYTE *t_h_a)
{
	memcpy(target_hardware_address, t_h_a, 6);
}

void ARP::setTargetProtocolAddress(DWORD t_p_a)
{
	target_protocol_address = t_p_a;
}

WORD ARP::getOperationCode()
{
	return operation_code;
}

BYTE *ARP::getSourceHardwareAddress()
{
	return source_hardware_address;
}

DWORD ARP::getSourceProtocolAddress()
{
	return source_protocol_address;
}