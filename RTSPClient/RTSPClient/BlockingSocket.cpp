#include "stdafx.h"
#include "BlockingSocket.h"

CBlockingSocket::CBlockingSocket()
{
	m_socket = INVALID_SOCKET;
}

CBlockingSocket::CBlockingSocket(SOCKET socket)
{
	m_socket = socket;
}

int CBlockingSocket::Initialize()
{
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	return 0;
}

int CBlockingSocket::Open(int transport)
{
	// Create a SOCKET for the server to listen for client connections
	switch(transport)
	{
	case 0:
		m_socket = socket(AF_INET, SOCK_STREAM, 0);
		break;
	case 1:
		m_socket = socket(AF_INET, SOCK_DGRAM, 0);
		break;
	default:
		break;
	}
	

	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		return 1;
	}

	return 0;
}

int CBlockingSocket::Bind(const char *ip, const char *port)
{
	sockaddr_in addr;
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));

	int iResult;

    // Connect to server.
    iResult = bind(m_socket, (sockaddr *)&addr, sizeof(sockaddr));
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error %d\n", WSAGetLastError());
        return 1;
    }

	return 0;
}

int CBlockingSocket::Connect(const char *ip, const char *port)
{
	sockaddr_in addr;
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));

	int iResult;

    // Connect to server.
    iResult = connect(m_socket, (sockaddr *)&addr, sizeof(sockaddr));
    if (iResult == SOCKET_ERROR) {
        printf("Unable to connect to server!\n");
        return 1;
    }

	return 0;
}

int CBlockingSocket::Send(const char *sendbuf, int sendbuflen)
{
	int iResult;

	// Send an initial buffer
	iResult = send(m_socket, sendbuf, sendbuflen, 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		return 1;
	}

	return 0;
}

int CBlockingSocket::Recv(char *recvbuf, int recvbuflen)
{
	int iResult;
    iResult = recv(m_socket, recvbuf, recvbuflen, 0);
    if (iResult > 0)
	{
        printf("Bytes received: %d\n", iResult);
		return iResult;
	}
    else if (iResult == 0)
	{
        printf("Connection closed\n");
		return -1;
	}
    else
	{
        printf("recv failed: %d\n", WSAGetLastError());
		return -1;
	}
}

int CBlockingSocket::RecvFrom(char *RecvBuf, int BufLen)
{
	int iResult;
	sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof (SenderAddr);

	wprintf(L"Receiving datagrams...\n");
    iResult = recvfrom(m_socket,
                       RecvBuf, BufLen, 0, (SOCKADDR *) & SenderAddr, &SenderAddrSize);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
		return 1;
    }
	return 0;
}

void CBlockingSocket::Close()
{
	closesocket(m_socket);
}

void CBlockingSocket::Cleanup()
{
	WSACleanup();
}

int CBlockingSocket::getSendRecvBuffer() const
{
	int nErrCode;
	int nBufLen;
	int nOptlen = sizeof(nBufLen);
	nErrCode = getsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char *)&nBufLen,&nOptlen);
	if (nErrCode == SOCKET_ERROR)
	{
		return 0;
	}
	return nBufLen;
}

int CBlockingSocket::setSendRecvBuffer(int nBufLen)
{
	int nErrCode;
	int nOptlen = sizeof(nBufLen);
	nErrCode = setsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char *)&nBufLen,nOptlen);
	if (nErrCode == SOCKET_ERROR)
	{
		return 1;
	}
	return 0;
}