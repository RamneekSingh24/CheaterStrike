#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "../game/Message.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include <string>

#define BUFLEN 1024

class Network
{

public:
	int send(Message msg);
	int recv(Message* msg);
	
	int init(std::string serverIP, int serverPort);

private:
	SOCKET sock;
	sockaddr_in server_addr;
	
};

