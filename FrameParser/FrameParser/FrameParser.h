#pragma once

#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "common.h"

#pragma pack(1)

typedef struct Frame{
	BYTE preamble[7];
	BYTE start;
	BYTE destination_address[6];
	BYTE source_address[6];
	BYTE type[2];
	BYTE data[46];
	BYTE fcs[4];
}MYFRAME, *PMYFRAME;

#pragma pack()

class FrameParser
{
public:
	FrameParser();
	~FrameParser();
	void setFrame(char *buffer);
	void printFrame(int num);
private:
	PMYFRAME m_frame;
};