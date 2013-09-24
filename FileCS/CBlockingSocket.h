#pragma once

//winsock 2.2
#include <winsock2.h>
//TCP/IP specific extensions in Windows Sockets 2
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class CBlockingSocket
{
private:
	SOCKET m_socket;
	struct addrinfo *m_result;
public:
	CBlockingSocket(void);
	CBlockingSocket(SOCKET socket);
	virtual ~CBlockingSocket(void);
	static int Initialize();
	int HintsAndResult(const char *ip, const char *port);
	int Open();
	int Bind();
	int Listen();
	CBlockingSocket Accept(struct sockaddr &their_addr);
	int Connect();
	int Send(const char *sendbuf, int sendbuflen);
	int Recv(char *recvbuf, int recvbuflen);
	void Close();
	static void Cleanup();
	int getSendRecvBuffer() const;
	int setSendRecvBuffer(const int nBufLen);
};