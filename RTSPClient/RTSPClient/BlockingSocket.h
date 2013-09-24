#pragma once

#include <winsock2.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class CBlockingSocket
{
private:
	SOCKET m_socket;
public:
	CBlockingSocket();
	CBlockingSocket(SOCKET socket);
	static int Initialize();
	int Open(int transport);
	int Bind(const char *ip, const char *port);
	int Connect(const char *ip, const char *port);
	int Send(const char *sendbuf, int sendbuflen);
	int Recv(char *recvbuf, int recvbuflen);
	int RecvFrom(char *recvbuf, int recvbuflen);
	void Close();
	static void Cleanup();
	int getSendRecvBuffer() const;
	int setSendRecvBuffer(int nBufLen);
};