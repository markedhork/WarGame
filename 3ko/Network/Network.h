#pragma once

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <string>
#include <thread>
#include <d3dx9.h>
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "3504"

const char OPTION_VALUE = 1;

enum SC_NUM
{
	TITLE_NUM,
	HOSTROOM_NUM,
	JOINTOTHER_NUM,
	GAME_NUM,
	END_NUM,
};

struct MY_MSG
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 rot;
};

//Type:1 Character Pos
//Type:2 Generate ball Pos
//Type:3 



class Network
{
public:
	Network();
	~Network();
	virtual bool Set() = 0;
	virtual void Process() = 0;
	virtual void Send(MY_MSG *p) = 0;
	virtual bool IfNewMsg() = 0;
	virtual MY_MSG GetMsg() = 0;

	LPCTSTR LocalIP = "NONE";

	int GetType();
	bool IfConected();
protected:
	bool Initialize();
	bool Uninitialize();
	WSAData wsa_data;
	std::thread connect_thread;
	std::thread process_thread;
	std::thread receive_thread;
	int type;
	bool Successed = false;

	
};

class Server :
	public Network
{
public:
	Server();
	~Server();
	bool Set();
	void Process();
	void Connect();
	void Send(MY_MSG *data);

	bool IfNewMsg();
	MY_MSG GetMsg();


private:
	void ProcessClient();
	addrinfo hints;
	addrinfo *server = NULL;
	SOCKET server_socket = INVALID_SOCKET;
	SOCKET client_socket = INVALID_SOCKET;

	bool NewMsg = false;
	MY_MSG MsgFromClient;
	LPCTSTR GetIP();

};

class Client :
	public Network
{
public:
	Client();
	~Client();
	bool Set();
	void Process();
	void Connect();
	void Send(MY_MSG *data);

	bool IfNewMsg();
	MY_MSG GetMsg();



private:
	void ProcessClient();
	addrinfo *result = NULL, hints, *ptr;
	SOCKET server_socket = INVALID_SOCKET;

	bool NewMsg = false;
	MY_MSG MsgFromServer;
};
