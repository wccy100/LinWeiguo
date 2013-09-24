#include "../CBlockingSocket.h"
#include <fstream>
#include <process.h>

const int BDP = 640000;//BDP = link_bandwidth * RTT
const int FPL = 256;

unsigned __stdcall tcpEchod(void *pArguments)
{
	CBlockingSocket *cs = (CBlockingSocket *)pArguments;
	CBlockingSocket ClientSocket = *cs;//用局部变量保存线程传进来的地址传递的参数，防止主线程中socket被改写。

	char filename[FPL] = {0};
	ClientSocket.Recv(filename,sizeof(filename));
	cout << "  File requested from the client： " << filename << endl;

	fstream fs;
	fs.open(filename, ios::in|ios::binary);
	if(!fs)
	{
		cout << "  " << filename << " does not exist." << endl;
	}
	else
	{
		fs.seekg(0,ios::end);
		int filelen = fs.tellg();
		fs.seekg(0,ios::beg);

		char buffer[BDP] = {0};
		if(filelen <= BDP)
		{
			fs.read(buffer,filelen);
			ClientSocket.Send(buffer,filelen);
		}
		else
		{
			int temp = filelen;
			while(temp > BDP)
			{
				fs.read(buffer,BDP);
				ClientSocket.Send(buffer,BDP);
				temp -= BDP;
			}
			fs.read(buffer,temp);
			ClientSocket.Send(buffer,temp);
		}
		cout << "  Sent file back to the client: " << filename << endl;
	}

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
	ListenSocket.setSendRecvBuffer(BDP);
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
	    //WaitForSingleObject( hThread, INFINITE );//加上这句就不是并发服务器了
	}

	ListenSocket.Close();
	CBlockingSocket::Cleanup();
	return 0;
}