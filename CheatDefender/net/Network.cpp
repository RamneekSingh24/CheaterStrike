#include "Network.h";
#include <iostream>
int Network::send(Message msg)
{


	char message[BUFLEN];
	memcpy(message, &msg, sizeof(msg));

	int flags = 0;
	if (sendto(sock, message, sizeof(message), flags, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		printf("sendto failed: %d", WSAGetLastError());

		return EXIT_FAILURE;
	}

	return 0;

}


int Network::recv(Message* msg) {



	char buffer[BUFLEN];
	int flags = 0;
	SOCKADDR_IN from;
	int from_size = sizeof(from);
	int bytes_received = recvfrom(sock, buffer, sizeof(buffer), flags, (SOCKADDR*)&from, &from_size);

	memcpy(msg, buffer, sizeof(Message));

	//if (bytes_received == SOCKET_ERROR) {
	//	return SOCKET_ERROR;
	//}
	//if (bytes_received == 0) {
	//	return 0;
	//}
	//if (from.sin_addr.S_un.S_addr == server_addr.sin_addr.S_un.S_addr) {
	//	// TODO: Decrypt buf
	//	memcpy(msg, buffer, sizeof(msg));
	//}
	return bytes_received;
}

int Network::init(std::string serverIP, int serverPort)
{


	// TODO: Exchange Ecnryption keys and save in enclave



	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("\nInitialising Winsock...Failed. Error Code : %d", WSAGetLastError());
		return EXIT_FAILURE;
	}

	struct sockaddr_in si_other;


	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		return EXIT_FAILURE;
	}


	SOCKADDR_IN local_address;
	local_address.sin_family = AF_INET;
	local_address.sin_port = htons(7878);
	local_address.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR)
	{
		printf("bind failed: %d", WSAGetLastError());
		return EXIT_FAILURE;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serverPort);
	server_addr.sin_addr.S_un.S_addr = inet_addr(serverIP.c_str());


	return 0;
}
