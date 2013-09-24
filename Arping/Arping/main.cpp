#include "Common.h"
#include "Device.h"
#include "EthernetFrame.h"
#include "ARP.h"

int main(int argc, char *argv[])
{
	//argc = 2;
	//argv[0] = "D:\\Arping\\Debug\\Arping.exe";
	//argv[1] = "222.31.76.151";

	if(argc != 2)
	{
		cout << "D:\\Arping.exe 222.31.76.139" << endl;
		return -1;
	}

	if(INADDR_NONE == inet_addr(argv[1]))
	{
		cout << "Invalid IP" << endl;
		return -1;
	}

	Device dev;

	//Obtaining the device list
	if(dev.obtainDeviceList())
		return -1;
	
	//Opening an adapter
	if(dev.openAdapter())
		return -1;
	
	//make arp request packet
	BYTE DestMac[6] = {0};
	BYTE SourceMac[6] = {0};
	memset(DestMac, 0xFF,6);//broadcast
	Common::getLocalMac(SourceMac);
	DWORD DestIp = inet_addr(argv[1]);
	DWORD SourceIp = dev.getIp();
	if(!SourceIp)//test the selected adapter is able to work
	{
		cout << "IP is 0.0.0.0, a wrong Adapter" << endl;
		return -1;
	}
	ARP a(SourceMac, SourceIp, DestMac, DestIp);
	EthernetFrame ef(DestMac, SourceMac, &a);

	//Filtering the traffic
	string strDestIp(argv[1]);
	string strSourceIp(Common::dword2Char(SourceIp));
	string filter = "(arp) and (host " + strSourceIp + " or host " + strDestIp + ")";
	if(dev.setFilter(filter.c_str()))
		return -1;

	//send arp request packet
	BYTE buf[60] = {0};
	ef.frame2Byte(buf);
	cout << "ARPING " << argv[1] << " from " << Common::dword2Char(SourceIp) << " eth0" << endl;
	for(int i=0; i<4; ++i)
	{
		dev.sendPacket(buf);
	}

	//receive arp request and reply packet thread
    DWORD   dwThreadId;
    HANDLE  hThread = CreateThread( 
        NULL,                   // default security attributes
        0,                      // use default stack size  
		(LPTHREAD_START_ROUTINE)dev.recvPacket(NULL),       // thread function name
        NULL,          // argument to thread function 
        0,                      // use default creation flags 
        &dwThreadId);   // returns the thread identifier 

	// Check the return value for success.
    // If CreateThread fails, terminate execution. 
    // This will automatically clean up threads and memory. 
	if(NULL == hThread)
	{
        printf("CreateThread failed (%d)\n", GetLastError());
        return -1;
	}

	exit(0);
	//return 0;无法退出，可能是Device不能正确析构。
}