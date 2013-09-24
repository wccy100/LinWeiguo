#include "CBlockingSocket.h"

CBlockingSocket::CBlockingSocket(void)
{
	m_socket = INVALID_SOCKET;
	m_result = NULL;
}

CBlockingSocket::CBlockingSocket(SOCKET socket)
{
	m_socket = socket;
	m_result = NULL;
}

CBlockingSocket::~CBlockingSocket(void)
{
	//if(m_result)
		//freeaddrinfo(m_result);
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

int CBlockingSocket::HintsAndResult(const char *ip, const char *port)
{
	int iResult;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (ip == NULL)
		hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(ip, port, &hints, &m_result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		return 1;
	}

	return 0;
}

int CBlockingSocket::Open()
{
	// Create a SOCKET for the server to listen for client connections
	m_socket = socket(m_result->ai_family, m_result->ai_socktype, m_result->ai_protocol);

	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		return 1;
	}

	return 0;
}

int CBlockingSocket::Bind()
{
	int iResult;

    // Setup the TCP listening socket
    iResult = bind( m_socket, m_result->ai_addr, (int)m_result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        return 1;
    }

	return 0;
}

int CBlockingSocket::Listen()
{
	if ( listen( m_socket, 5 ) == SOCKET_ERROR ) {
		printf( "Listen failed with error: %ld\n", WSAGetLastError() );
		return 1;
	}

	return 0;
}

CBlockingSocket CBlockingSocket::Accept(struct sockaddr &their_addr)
{
	int addr_size = sizeof(their_addr);
	SOCKET ClientSocket;
	ClientSocket = INVALID_SOCKET;

	fd_set fdread;
	int ret;
	FD_ZERO(&fdread);
	FD_SET(m_socket, &fdread);
	
	if( (ret = select(0, &fdread, NULL, NULL, NULL)) == SOCKET_ERROR )
	{
		return INVALID_SOCKET;
	}

	if(ret > 0)
	{
		if(FD_ISSET(m_socket, &fdread))
		{
			// Accept a client socket
			ClientSocket = accept(m_socket, &their_addr, &addr_size);
			if (ClientSocket == INVALID_SOCKET) {
				printf("accept failed: %d\n", WSAGetLastError());
				return INVALID_SOCKET;
			}

			return CBlockingSocket(ClientSocket);
		}
	}

	return INVALID_SOCKET;
}

int CBlockingSocket::Connect()
{
	int iResult;

    // Connect to server.
    iResult = connect( m_socket, m_result->ai_addr, (int)m_result->ai_addrlen);
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
        //printf("Bytes received: %d\n", iResult);
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

int CBlockingSocket::setSendRecvBuffer(const int nBufLen)
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