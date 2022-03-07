#include "Network.h"
#include <iostream>

int Network::send(Message msg, std::string clientIP, int clientPort)
{

	sockaddr_in recvr_addr;

	recvr_addr.sin_family = AF_INET;
	recvr_addr.sin_port = htons(clientPort);
	recvr_addr.sin_addr.S_un.S_addr = inet_addr(clientIP.c_str());

	// TODO Encrypt buf
	char message[BUFLEN];
	memcpy(message, &msg, sizeof(msg));

	int flags = 0;
	if (sendto(sock, message, sizeof(message), flags, (SOCKADDR*)&recvr_addr, sizeof(recvr_addr)) == SOCKET_ERROR)
	{
		printf("sendto failed: %d", WSAGetLastError());

		return EXIT_FAILURE;
	}



	return 0;
}


int Network::recv(Message* msg, std::string &clientIP, int &clientPort) {

	char buf[BUFLEN];
	int flags = 0;
	SOCKADDR_IN from;
	int from_size = sizeof(from);
	int bytes_received = recvfrom(sock, buf, sizeof(buf), flags, (SOCKADDR*)&from, &from_size);
	memcpy(msg, buf, sizeof(Message));

	if (bytes_received == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}
	if (bytes_received == 0) {
		return 0;
	}

	// TODO: Decrypt buf
	memcpy(msg, buf, sizeof(msg));
	char fromIP[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(from.sin_addr), fromIP, INET_ADDRSTRLEN);
	clientIP = std::string(fromIP);
	clientPort = ntohs(from.sin_port);

	return bytes_received;
}

int Network::init(std::string myIP, int myPort)
{


	// TODO: Exchange Ecnryption keys and save in enclave



	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("\nInitialising Winsock...Failed. Error Code : %d", WSAGetLastError());
		return EXIT_FAILURE;
	}



	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		return EXIT_FAILURE;
	}


	SOCKADDR_IN local_address;
	local_address.sin_family = AF_INET;
	local_address.sin_port = htons(myPort);
	local_address.sin_addr.s_addr = inet_addr(myIP.c_str());
	if (bind(sock, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR)
	{
		printf("bind failed: %d", WSAGetLastError());
		return EXIT_FAILURE;
	}



	return 0;
}

