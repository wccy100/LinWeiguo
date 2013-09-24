#include <winsock2.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main(int argc, char *argv[])
{
	//argc = 2;
	//argv[0] = "..\\EchoCS\\Debug\\EchoServer.exe";
	//argv[1] = "8888";

	if(argc != 2)
	{
		cout << "EchoServer 8888" << endl;
		return -1;
	}

	// Initialize Winsock version 2.2
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData)!= 0)
	{
		cout << "WSAStartup failed" << endl;
		return -1;
	}

	//Creating a Socket
	SOCKET listenSocket, acceptSocket;
	listenSocket = socket(AF_INET,SOCK_STREAM,0);
	if (listenSocket == INVALID_SOCKET)
	{
		printf( "Socket failed with error %d\n", WSAGetLastError());
		return -1;
	}

	//Bind socket
	struct sockaddr_in ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ServerAddr.sin_port = htons(atoi(argv[1]));
	bind (listenSocket ,(struct sockaddr *) &ServerAddr, sizeof(ServerAddr));

	//Listen to incoming requests
	listen(listenSocket, 5);
	char name[256];
	gethostname(name, sizeof(name));
	hostent *remoteHost = gethostbyname(name);
	struct in_addr addr;
	addr.s_addr = *(u_long *) remoteHost->h_addr_list[0];
	cout << "EchoServer is listening on " << inet_ntoa(addr) << ":" << argv[1] << endl;

	//Accept Incoming Connection
	struct sockaddr_in ClientAddr;
	int Size = sizeof(ClientAddr);
	char buf[256];
	while(1)
	{
		cout << "Waiting for a connection¡­" << endl;
		memset(buf, 0, sizeof(buf));
		acceptSocket = accept(listenSocket, (struct sockaddr *) &ClientAddr, &Size);
		addr = ClientAddr.sin_addr;
		cout << "Accepted connection from " << inet_ntoa(addr) << endl;

		//Receiving data
		int bytesrecv = recv(acceptSocket, buf, sizeof(buf), 0);
		cout << "  Received " << bytesrecv << " bytes from the client£º " << buf << endl;

		//Sending Data
		int bytessend = send ( acceptSocket, buf, strlen(buf), 0);
		if (bytessend < 0)
		{
			printf("SOCKET END ERROR: %d\n", GetLastError());
			return -1;
		}
		cout << "  Sent " << bytessend << " bytes back to the client: " << buf << endl;
		cout << "Connection closed by the client" << endl;
	}

	shutdown(acceptSocket, SD_BOTH);
	closesocket(acceptSocket);
	shutdown(listenSocket, SD_BOTH);
	closesocket(listenSocket);

	if (WSACleanup() == SOCKET_ERROR)
	{
		printf("WSACleanup failed with error %d\n", WSAGetLastError());
		return -1;
	}

	return 0;
}