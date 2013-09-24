#pragma once

#include "IP.h"

class ICMP:public IP
{
protected:
	BYTE m_type;
	BYTE m_code;
	WORD m_checksum;
	WORD m_id;
	WORD m_seq;
	BYTE m_data[64];
public:
	virtual int GetTotalLength() const{return 0;}
	virtual void SetTotalLength(int length){;}
	virtual int GetHeaderLength() const{return 0;}
	virtual void SetHeaderLength(int length){;}
	virtual void PrintHeader() const{;}
	virtual void Print() const{;}
	virtual BOOL Write(BYTE *link);
	virtual BOOL Read(BYTE *link);

	void setType(BYTE type);
	void setCode(BYTE code);
	void setChecksum(WORD checksum);
	void setId(WORD id);
	void setSeq(WORD seq);
	void setData(BYTE data[]);

	BYTE getType() const;
	BYTE getCode() const;
	WORD getChecksum() const;
	WORD getId() const;
	WORD getSeq() const;
	const BYTE *getData() const;
};