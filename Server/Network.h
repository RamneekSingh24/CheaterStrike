#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "../CheatDefender/game/Message.h"
#include "../CheatDefender/game/Record.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "aes.h"

#define BUFLEN 1024

class Network
{

public:
	int send(Message msg, std::string toIP, int clientPort, unsigned char* shared_secret);
	int recv(Message* msg, std::string &clientIP, int &clientPort);

	int init(std::string serverIP, int serverPort, bool encryption);

private:
	SOCKET sock;
	std::unordered_map<std::string, int> send_seq_no;
	std::unordered_map<std::string, AES_ctx> aes_ctxs;
	bool encryptionOn;
	//std::unordered_map<std::string, int> reqv_seq_no;  Don't need to encrypt msgs from client to server.
};

