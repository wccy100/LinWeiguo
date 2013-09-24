#pragma once

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef UINT
typedef unsigned int UINT;
#endif

using namespace std;

class Common
{
public:
	static void compareFCS(char *fcs1, UINT fcs2)
	{
		int result = memcmp(fcs1, &fcs2, 4);
		if(!result)
			cout << "CRC32 Check Correct !" << endl;
		else
		{
			cout << "CRC32 Check Wrong !" << endl;
			cout << "Recalculate Result:    0x " << fcs2 << endl;
		}
	}
};