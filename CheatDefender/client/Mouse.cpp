#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Mouse.h"
#include <thread>




void Mouse::recv() {

	while (true) {
		char buffer[BUFLEN];
		int flags = 0;
		SOCKADDR_IN from;
		int from_size = sizeof(from);
		int bytes_received = recvfrom(sock, buffer, sizeof(buffer), flags, (SOCKADDR*)&from, &from_size);

		unsigned char input_with_auth_tag[24];
		memcpy(input_with_auth_tag, buffer, 24); 

		enc_w->verifyInput(input_with_auth_tag);

		int deltaX, deltaY;

		static_assert(sizeof(int) == 4, "sizeof(int) != 4");
		memcpy(&deltaX, buffer, sizeof(int));
		memcpy(&deltaY, buffer + sizeof(int), sizeof(int));



		if (deltaX == 0 && deltaY == 0) {
			continue;
		}
		
		queueLock.lock();
		mouseInputs.push(std::make_pair(static_cast<float> (deltaX), static_cast<float> (deltaY)));
		queueLock.unlock();
	}

}



std::pair<float, float> Mouse::getXYChangeInput()
{
	queueLock.lock();
	if (mouseInputs.empty()) {
		queueLock.unlock();
		return std::make_pair(0.0f, 0.0f);
	}
	else {
		std::pair<float, float> inp = mouseInputs.front();
		mouseInputs.pop();
		queueLock.unlock();
		return inp;
	}



}

void Mouse::init(bool inputProtectionOn, EnclaveWrapper* enc_w)
{

	std::string inputServerIp = "127.0.0.1";
	int inputServerPort = 8765;

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("\nInitialising Winsock...Failed. Error Code : %d", WSAGetLastError());
		return;
	}

	struct sockaddr_in si_other;


	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		return;
	}


	SOCKADDR_IN local_address;
	local_address.sin_family = AF_INET;
	local_address.sin_port = htons(11111);
	local_address.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR)
	{
		printf("bind failed: %d", WSAGetLastError());
		return;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(inputServerPort);
	server_addr.sin_addr.S_un.S_addr = inet_addr(inputServerIp.c_str());

	this->enc_w = enc_w;


	char message[BUFLEN];
	memcpy(message, &enc_w->enclavePublicKey, 64);

	int flags = 0;
	if (sendto(sock, message, sizeof(message), flags, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		printf("sendto failed: %d", WSAGetLastError());
		return;
	}


	char buffer[BUFLEN];
	flags = 0;
	SOCKADDR_IN from;
	int from_size = sizeof(from);
	int bytes_received = recvfrom(sock, buffer, sizeof(buffer), flags, (SOCKADDR*)&from, &from_size);

	unsigned char peer_pub_key[64];
	memcpy(peer_pub_key, buffer, 64);

	enc_w->deriveAuthKey(peer_pub_key);


	std::thread* th = (new std::thread(&Mouse::recv, this));


}
