#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "../CheatDefender/game/Message.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include <string>

#define BUFLEN 1024

class Network
{

public:
	int send(Message msg, std::string toIP, int clientPort);
	int recv(Message* msg, std::string &clientIP, int &clientPort);

	int init(std::string serverIP, int serverPort);

private:
	SOCKET sock;


};

