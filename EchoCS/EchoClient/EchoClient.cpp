#include <winsock2.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main(int argc, char *argv[])
{
	//argc = 3;
	//argv[0] = "..\\EchoCS\\Debug\\EchoClient.exe";
	//argv[1] = "10.128.6.250";
	//argv[2] = "8888";

	if(argc != 3)
	{
		cout << "EchoClient 192.168.39.120 8888" << endl;
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
	SOCKET connectSocket;
	connectSocket = socket(AF_INET,SOCK_STREAM,0);
	if (connectSocket == INVALID_SOCKET)
	{
		printf( "Socket failed with error %d\n", WSAGetLastError());
		return -1;
	}

	//Connecting
	struct sockaddr_in ServerAddr;
	ServerAddr.sin_family = AF_INET;
	struct in_addr addr;
	addr.s_addr = inet_addr(argv[1]);
	ServerAddr.sin_addr = addr;
	ServerAddr.sin_port = htons(atoi(argv[2]));
	connect ( connectSocket, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr) );
	cout << "Connection established to remote Server at "<< argv[1] << ":" << argv[2] << endl;
	
	//Sending Data
	char message[256] = {0};
	cin.getline(message, sizeof(message));
	int bytessend = send ( connectSocket, message, strlen(message), 0);
	if (bytessend < 0)
	{
		printf("SOCKET END ERROR: %d\n", GetLastError());
		return -1;
	}
	cout << "  Sent "<< bytessend << " bytes to the server: " << message << endl;

	//Receiving data
	char buf[256] = {0};
	int bytesrecv = recv(connectSocket, buf, sizeof(buf), 0);
	cout << "  Received " << bytesrecv << " bytes from the server, Reply matches what we sent!" << endl;

	cout << "  Shutdown the connection in 3 seconds¡­¡­" << endl;
	Sleep(3000);
	shutdown(connectSocket, SD_BOTH);
	closesocket(connectSocket);
	cout << "Connection is down." << endl;

	if (WSACleanup() == SOCKET_ERROR)
	{
		printf("WSACleanup failed with error %d\n", WSAGetLastError());
		return -1;
	}

	return 0;
}