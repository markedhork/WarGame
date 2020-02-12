#include "Network.h"

Client::Client()
{
	this->type = JOINTOTHER_NUM;
}

Client::~Client()
{
	int iResult = shutdown(server_socket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		OutputDebugStringA("shutdown() failed with error\n");
	}
	this->process_thread.detach();
	this->connect_thread.detach();
	closesocket(server_socket);

}

bool Client::Set()
{
	//Setup hints
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	int iResult = getaddrinfo(LocalIP, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		OutputDebugStringA("getaddrinfo() failed in Client class");
		return false;
	}
	this->Process();
	return true;
}

void Client::Process()
{
	this->connect_thread = std::thread(&Client::Connect, this);
}

void Client::Connect()
{
	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (server_socket == INVALID_SOCKET) {
			int error = WSAGetLastError();
			std::string outmsg = "socket() failed Error: ";

			outmsg += std::to_string(error);
			outmsg += "\n";
			OutputDebugStringA(outmsg.c_str());
			WSACleanup();
			return;
		}

		// Connect to server.
		int iResult = connect(server_socket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(server_socket);
			server_socket = INVALID_SOCKET;
			continue;
		}
	}

	freeaddrinfo(result);
	if (server_socket == INVALID_SOCKET) {
		OutputDebugStringA("Unable to connect to server!\n");
	}
	else
	{
		OutputDebugStringA("Successfully Connected\n");
	}
	this->process_thread = std::thread(&Client::ProcessClient, this);
}

void Client::Send(MY_MSG *data)
{
	if (server_socket != 0)
	{
		int iResult = send(server_socket, (char*)data, sizeof(MY_MSG), 0);
		if (iResult < 0)
		{
			OutputDebugStringA("Client send() to server failed\n");
		}
	}
}

bool Client::IfNewMsg()
{
	return this->NewMsg;
}

MY_MSG Client::GetMsg()
{
	MY_MSG temp;
	temp = MsgFromServer;
	this->NewMsg = false;
	return temp;
}


void Client::ProcessClient()
{
	this->Successed = true;

	while (1)
	{
		MY_MSG tempRecv;
		if (server_socket != 0)
		{
			int iResult = recv(server_socket, (char*)&tempRecv, sizeof(MY_MSG), 0);


			if (iResult != SOCKET_ERROR)
			{
				std::string Outmsg = "message from server";

				Outmsg += (char*)&tempRecv;
				Outmsg += "\n";
				OutputDebugStringA(Outmsg.c_str());

				this->MsgFromServer = tempRecv;
				this->NewMsg = true;
			}
			else
			{
				std::string Outmsg = "";
				Outmsg += "client recv() from server failed: ";
				Outmsg += std::to_string(WSAGetLastError());
				Outmsg += "\n";
				OutputDebugStringA(Outmsg.c_str());
				break;
			}
		}
	}

	if (WSAGetLastError() == WSAECONNRESET)
	{
		OutputDebugStringA("The server has disconnected\n");
		this->Successed = false;
	}
}
