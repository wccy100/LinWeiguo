#include "FrameParser.h"
#include "CCRC32.H"

int main(int argc, char *argv[])
{
	//argc = 2;
	//argv[0] = "D:\\FrameParser\\Debug\\FrameParser.exe";
	//argv[1] = "D:\\testFrame";

	if(argc != 2)
	{
		cout << "D:\\FrameParser.exe testFrame" << endl;
		return 1;
	}

	ifstream file(argv[1], ios::in | ios::binary);
	if (!file.is_open())
	{
		cout << "Can't open the file" << endl;
		return 1;
	}

	char *buffer = new char[sizeof(MYFRAME)];
	
	FrameParser fp;

	CCRC32 crc32;
	crc32.Initialize(); //Only have to do this once.

	for(int i=0; i<3; i++)
	{
		file.seekg(i*sizeof(MYFRAME), ios::beg);
		file.read(buffer, sizeof(MYFRAME));
		fp.setFrame(buffer);
		fp.printFrame(i+1);
		UINT myfcs = crc32.FullCRC((BYTE *)buffer+8, 60);
		Common::compareFCS(buffer+68, myfcs);
		cout << "----------------------------------------------" << endl << endl;
	}
	cout << "Analysis Finished!" << endl;

	delete[] buffer;
	buffer = NULL;

	return 0;
}