#include "../../Common.h"
#include "../../CBlockingSocket.h"
#include "../../Message.h"
#include <fstream>

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
	ConnectSocket.setSendRecvBuffer(12+BDP);
	ConnectSocket.Connect();
	cout << "Connection established to remote Server at " << argv[1] << ":" << argv[2] << endl;

	//1. The client sends a request to the server to ask for the size of a specific file.
	char filename[FPL] = {0};
	cout << "Input file path:";
	cin >> filename;
	Message msg(FILE_SIZE_REQUEST, 12+FPL, 0, 0, filename);
	char buffer[12+FPL] = {0};
	msg.write(buffer);
	ConnectSocket.Send(buffer, msg.getTotalLength());
	cout << "  Requesting file on the server: " << filename << endl;

	//2. The server sends back a reply with the size of the file if file exists.
	ConnectSocket.Recv(buffer,msg.getTotalLength());
	msg.read(buffer);
	int filelen = msg.getSize();
	if(filelen == -1)
	{
		cout << "  "<< filename << " does not exist." << endl;
		return 1;
	}

	//3. The client creates a separate thread, and send request for the file data (optional implementation: the
	//   client may create 2 or more threads for downloading, each thread for a part of the file in a separate
	//   connection).
	cout << "  Receiving file " << filename << endl;
	msg.setType(FILE_DATA_REQUEST);
	msg.setTotalLength(12+strlen(filename));
	msg.setSize(BDP);
	msg.write(buffer);
	ConnectSocket.Send(buffer, 12+strlen(filename));

	char filename2[FPL] = {0};
	cout << "  Input save path:";
	cin >> filename2;

	fstream fs;
	locale::global(locale(""));//将全局区域设为操作系统默认区域
	//setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
	fs.open(filename2, ios::out|ios::binary);
	locale::global(locale("C"));//还原全局区域设定
	//setlocale(LC_ALL,"C");//还原

	//4. The server accepts the data download request, and starts a new thread to send the file to the client
	//   (if the client uses more than 1 thread for downloading, the server should start more than 1 thread
	//   for the corresponding connection request).
	char buffer2[12+BDP] = {0};
	int filerecvd = 0;
	while(1)
	{
		filerecvd = ConnectSocket.Recv(buffer2,sizeof(buffer2));
		if(filerecvd == -1)
			break;
		cout << "  Received " << filerecvd-12 << " bytes." << endl;
		fs.write(buffer2+12, filerecvd-12);
	}

	fs.close();
	ConnectSocket.Close();
	cout << "Connection is down." << endl;
	CBlockingSocket::Cleanup();
	return 0;
}