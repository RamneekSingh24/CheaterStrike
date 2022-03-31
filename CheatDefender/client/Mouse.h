#pragma once
#define NOMINMAX
#include "Window.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include "../EnclaveWrapper.h"

#define BUFLEN 1024


class Mouse {
public:
	
	std::pair<float, float> getXYChangeInput();

	void init(bool inputProtectionOn, EnclaveWrapper* enc_w);

	SOCKET sock;
	SOCKADDR_IN  server_addr;
private:
	float x, y;
	void recv();
	std::queue<std::pair<float, float>> mouseInputs;
	std::mutex queueLock;
	EnclaveWrapper* enc_w;
};