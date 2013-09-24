#include "../CBlockingSocket.h"
#include <fstream>

const int BDP = 640000;//BDP = link_bandwidth * RTT
const int FPL = 256;

int main(int argc, char *argv[])
{
	//argc = 3;
	//argv[0] = "..\\Debug\\FileClient.exe";
	//argv[1] = "10.37.104.242";
	//argv[2] = "8888";

	if(argc != 3)
	{
		cout << "FileClient 192.168.39.1 8888" << endl;
		return 1;
	}

    CBlockingSocket ConnectSocket;

	CBlockingSocket::Initialize();
	ConnectSocket.HintsAndResult(argv[1], argv[2]);
	ConnectSocket.Open();
	ConnectSocket.setSendRecvBuffer(BDP);
	ConnectSocket.Connect();
	cout << "Connection established to remote Server at " << argv[1] << ":" << argv[2] << endl;

	char filename[FPL] = {0};
	cout << "Input file path:";
	cin >> filename;
	ConnectSocket.Send(filename,strlen(filename));
	cout << "  Requesting file on the server: " << filename << endl;

	cout << "  1.save the file to disk" << endl;
	cout << "  2.print out on the console" << endl;
	cout << "  Input your choise:";
	int choose;
	cin >> choose;

	char buffer[BDP] = {0};
	if(choose == 1)
	{
		char filename2[FPL] = {0};
		cout << "  Input save path:";
		cin >> filename2;

		cout << "  Receiving file " << filename << endl;
		fstream fs;
		fs.open(filename2, ios::out|ios::binary);
		int filerecvd = 0;
		while(filerecvd != -1)
		{
			filerecvd = ConnectSocket.Recv(buffer,sizeof(buffer));
			cout << "  Received " << filerecvd << " bytes." << endl;
			fs.write(buffer, filerecvd);
		}
	}
	else if(choose == 2)
	{
		int filerecvd = ConnectSocket.Recv(buffer,sizeof(buffer));
		cout << "  Receiving file " << filename << endl;
		cout << "  =====================" << endl;
		cout << "  " << buffer << endl;
		cout << "  =====================" << endl;
		cout << "  Received " << filerecvd << " bytes." << endl;
	}
	else
	{
		cout << "  Please input 1 or 2." << endl;
		return 1;
	}

	ConnectSocket.Close();
	cout << "Connection is down." << endl;
	CBlockingSocket::Cleanup();
	return 0;
}