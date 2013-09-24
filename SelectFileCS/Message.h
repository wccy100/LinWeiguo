#pragma once

#include <string.h>

static const char FILE_SIZE_REQUEST = 1;
static const char FILE_SIZE_REPLY = 1;
static const char FILE_DATA_REQUEST = 2;
static const char FILE_DATA_REPLY = 2;

typedef struct MsgHeader{
	char Type;					//Message Type,
	char Reserved;				//Reserved for future use
	unsigned short TotalLength; //Total Message Length including Header and Data
	unsigned long Position;		//Identities the relative position in the downloading file for the
								//1st byte of the Data field of the message
	unsigned long Size;			// a) the size of the File in a File Size Request/Rely Msg
								// b) the size of the total data starting from the Position field indicates in a File
								//	  Data Request Msg (could be used for multi-part downloading)
								// c) the size of the Data field of current Msg in a File Data Reply Msg
}MessageHeader;

class Message
{
private:
	MessageHeader m_MessageHeader;
	char *m_Data;
public:
	Message();
	Message(char Type, unsigned short TotalLength, unsigned long Position, unsigned long Size, char *Data, char Reserved=0);
	~Message();

	void setType(char Type);
	void setReserved(char Reserved=0);
	void setTotalLength(unsigned short TotalLength);
	void setPosition(unsigned long Position);
	void setSize(unsigned long Size);
	void setData(char *Data);

	char getType() const;
	char getReserved() const;
	unsigned short getTotalLength() const;
	unsigned long getPosition() const;
	unsigned long getSize() const;
	char *getData() const;

	void write(char *buffer);
	void read(char *buffer);
};