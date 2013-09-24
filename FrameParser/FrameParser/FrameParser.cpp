#include "FrameParser.h"

FrameParser::FrameParser()
{
	m_frame = new MYFRAME;
}

FrameParser::~FrameParser()
{
	delete m_frame;
	m_frame = NULL;
}

void FrameParser::setFrame(char *buffer)
{
	memcpy(m_frame->preamble, buffer, 7);
	memcpy(&m_frame->start, buffer+7, 1);
	memcpy(m_frame->destination_address, buffer+8, 6);
	memcpy(m_frame->source_address, buffer+14, 6);
	memcpy(m_frame->type, buffer+20, 2);
	memcpy(m_frame->data, buffer+22, 46);
	memcpy(m_frame->fcs, buffer+68, 4);
}

void FrameParser::printFrame(int num)
{
	cout << "Frame#:                " << num << endl;

	cout << "Preamble:              ";
	for(int i=0; i<7; ++i)
		cout << hex << uppercase << (UINT)m_frame->preamble[i] << " ";
	cout << endl;

	cout << "Start:                 " << (UINT)m_frame->start << endl;

	cout << "Destination Address:   ";
	for(int i=0; i<6; ++i)
	{
		cout << setw(2) << setfill('0') << (UINT)m_frame->destination_address[i];
		if(i != 5)
			cout << "-";
	}
	cout << endl;

	cout << "Source Address:        ";
	for(int i=0; i<6; ++i)
	{
		cout << setw(2) << setfill('0') << (UINT)m_frame->source_address[i];
		if(i != 5)
			cout << "-";
	}
	cout << endl;

	cout << "Type:                  ";
	for(int i=0; i<2; ++i)
		 cout << setw(2) << (UINT)m_frame->type[i] << " ";
	cout << endl;

	cout << "Data:                  " << m_frame->data << endl;

	cout << "FCS:                   0x ";
	for(int i=3; i>=0; --i)
		cout << hex << nouppercase << (UINT)m_frame->fcs[i];
	cout << endl;
}