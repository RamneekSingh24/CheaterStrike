#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "../game/Message.h"
#include "../game/Record.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include <string>
#include "../EnclaveWrapper.h"


#define BUFLEN 1024

class Network
{

public:
	int send(Message msg);
	int recv(Message* msg);

	int init(std::string serverIP, int serverPort, EnclaveWrapper* enc_w);

private:
	SOCKET sock;
	SOCKADDR_IN  server_addr;
	EnclaveWrapper* enc_w;
	// bool encryptionOn;    // not needed : server side messages will not have encryption flag set and we are checking it in client

};