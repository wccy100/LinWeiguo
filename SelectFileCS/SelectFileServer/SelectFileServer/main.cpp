#include "../../Common.h"
#include "../../CBlockingSocket.h"
#include "../../Message.h"
#include <fstream>
#include <process.h>

unsigned __stdcall tcpEchod(void *pArguments)
{
	CBlockingSocket *cs = (CBlockingSocket *)pArguments;
	CBlockingSocket ClientSocket = *cs;//用局部变量保存线程传进来的地址传递的参数，防止主线程中socket被改写。

	//1. The client sends a request to the server to ask for the size of a specific file.
	char filename[FPL] = {0};
	char buffer[12+FPL] = {0};
	ClientSocket.Recv(buffer,sizeof(buffer));
	Message msg;
	msg.read(buffer);
	memcpy(filename, msg.getData(), msg.getTotalLength()-12);
	cout << "  File requested from the client： " << filename << endl;

	fstream fs;
	locale::global(locale(""));//将全局区域设为操作系统默认区域
	//setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
	fs.open(filename, ios::in|ios::binary);
	locale::global(locale("C"));//还原全局区域设定
	//setlocale(LC_ALL,"C");//还原
	if(!fs)
	{
		cout << "  " << filename << " does not exist." << endl;
		msg.setSize(-1);
		msg.write(buffer);
		ClientSocket.Send(buffer,msg.getTotalLength());
	}
	else
	{
		//2. The server sends back a reply with the size of the file if file exists.
		fs.seekg(0,ios::end);
		int filelen = fs.tellg();
		fs.seekg(0,ios::beg);
		msg.setSize(filelen);
		msg.write(buffer);
		ClientSocket.Send(buffer,msg.getTotalLength());

		//3. The client creates a separate thread, and send request for the file data (optional implementation: the
		//   client may create 2 or more threads for downloading, each thread for a part of the file in a separate
		//   connection).
		ClientSocket.Recv(buffer,sizeof(buffer));
		msg.read(buffer);

		//4. The server accepts the data download request, and starts a new thread to send the file to the client
		//   (if the client uses more than 1 thread for downloading, the server should start more than 1 thread
		//	 for the corresponding connection request).
		char buffer2[BDP] = {0};
		char *buffer3 = new char[12+BDP];
		if(filelen <= BDP)
		{
			fs.read(buffer2,filelen);
			msg.setTotalLength(12+filelen);
			msg.setData(buffer2);
			msg.write(buffer3);
			ClientSocket.Send(buffer3,12+filelen);
		}
		else
		{
			int count = 0;
			for(int block=0; block<filelen/BDP; ++block)
			{
				fs.read(buffer2, BDP);
				msg.setTotalLength(12+(unsigned short)BDP);
				msg.setPosition(count);
				count += BDP;
				msg.setSize(BDP);
				msg.setData(buffer2);
				msg.write(buffer3);
				ClientSocket.Send(buffer3,12+BDP);
			}
			fs.read(buffer2, filelen%BDP);
			msg.setTotalLength(12+(unsigned short)filelen%BDP);
			msg.setPosition(count);
			msg.setSize(BDP);
			msg.setData(buffer2);
			msg.write(buffer3);
			ClientSocket.Send(buffer3,12+filelen%BDP);
		}
		//msg.setSize(0);
		//msg.setData(NULL);
		//msg.write(buffer3);
		//ClientSocket.Send(buffer3,12);
		cout << "  Sent file back to the client: " << filename << endl;
		delete[] buffer3;
	}
	fs.close();
	
	ClientSocket.Close();
	return 0;
}

int main(int argc, char *argv[])
{
	//argc = 2;
	//argv[0] = "..\\Debug\\FileServer.exe";
	//argv[1] = "8888";

	if(argc != 2)
	{
		cout << "FileServer 8888" << endl;
		return 1;
	}

    CBlockingSocket ListenSocket, ClientSocket;

	CBlockingSocket::Initialize();
	ListenSocket.HintsAndResult(NULL, argv[1]);
	ListenSocket.Open();
	ListenSocket.Bind();
	ListenSocket.setSendRecvBuffer(12+BDP);
	ListenSocket.Listen();
	cout << "FileServer is listening on port:" << argv[1] << endl;

	while(1)
	{
		cout << "Waiting for a connection…" << endl;

		struct sockaddr their_addr;
		struct sockaddr_in their_addrin;
		ClientSocket = ListenSocket.Accept(their_addr);
		memcpy(&their_addrin, &their_addr, sizeof(their_addr));
		cout << "Accepted connection from " << inet_ntoa(their_addrin.sin_addr) << endl;

		HANDLE hThread;
		unsigned threadID;
		hThread = (HANDLE)_beginthreadex( NULL, 0, &tcpEchod, &ClientSocket, 0, &threadID );
	}

	ListenSocket.Close();
	CBlockingSocket::Cleanup();
	return 0;
}