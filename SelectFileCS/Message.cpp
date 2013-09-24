#include "Message.h"

Message::Message()
{
	m_MessageHeader.Type = 0;
	m_MessageHeader.Reserved = 0;
	m_MessageHeader.TotalLength = 0;
	m_MessageHeader.Position = 0;
	m_MessageHeader.Size = 0;
	m_Data = NULL;
}

Message::Message(char Type, unsigned short TotalLength, unsigned long Position, unsigned long Size, char *Data, char Reserved)
{
	m_MessageHeader.Type = Type;
	m_MessageHeader.Reserved = Reserved;
	m_MessageHeader.TotalLength = TotalLength;
	m_MessageHeader.Position = Position;
	m_MessageHeader.Size = Size;
	m_Data = Data;
}

Message::~Message()
{

}

void Message::setType(char Type)
{
	m_MessageHeader.Type = Type;
}

void Message::setReserved(char Reserved)
{
	m_MessageHeader.Reserved = Reserved;
}

void Message::setTotalLength(unsigned short TotalLength)
{
	m_MessageHeader.TotalLength = TotalLength;
}

void Message::setPosition(unsigned long Position)
{
	m_MessageHeader.Position = Position;
}

void Message::setSize(unsigned long Size)
{
	m_MessageHeader.Size = Size;
}

void Message::setData(char *Data)
{
	m_Data = Data;
}

char Message::getType() const
{
	return m_MessageHeader.Type;
}

char Message::getReserved() const
{
	return m_MessageHeader.Reserved;
}

unsigned short Message::getTotalLength() const
{
	return m_MessageHeader.TotalLength;
}

unsigned long Message::getPosition() const
{
	return m_MessageHeader.Position;
}

unsigned long Message::getSize() const
{
	return m_MessageHeader.Size;
}

char *Message::getData() const
{
	return m_Data;
}

void Message::write(char *buffer)
{
	memcpy(buffer, &m_MessageHeader.Type, 1);
	memcpy(buffer+1, &m_MessageHeader.Reserved, 1);
	memcpy(buffer+2, &m_MessageHeader.TotalLength, 2);
	memcpy(buffer+4, &m_MessageHeader.Position, 4);
	memcpy(buffer+8, &m_MessageHeader.Size, 4);
	if(m_Data)
		memcpy(buffer+12, m_Data, m_MessageHeader.TotalLength-12);
}

void Message::read(char *buffer)
{
	memcpy(&m_MessageHeader.Type, buffer, 1);
	memcpy(&m_MessageHeader.Reserved, buffer+1, 1);
	memcpy(&m_MessageHeader.TotalLength, buffer+2, 2);
	memcpy(&m_MessageHeader.Position, buffer+4, 4);
	memcpy(&m_MessageHeader.Size, buffer+8, 4);
	m_Data = buffer+12;
}