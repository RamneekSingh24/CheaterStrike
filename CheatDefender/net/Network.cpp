#include "Network.h";

int Network::send(Message msg)
{
	// TODO Encrypt buf
	char buf[BUFLEN];
	memcpy(buf, &msg, sizeof(msg));

	if (sendto(sock, buf, sizeof(buf), 0, (struct sockaddr*)&server_addr, sizeof(server_addr) == SOCKET_ERROR))
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		return EXIT_FAILURE;
	}
}


int Network::recv(Message* msg) {
	char buf[BUFLEN];
	int flags = 0;
	SOCKADDR_IN from;
	int from_size = sizeof(from);
	int bytes_received = recvfrom(sock, buf, sizeof(buf), flags, (SOCKADDR*)&from, &from_size);
	if (bytes_received == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}
	if (bytes_received == 0) {
		return 0;
	}
	if (from.sin_addr.S_un.S_addr == server_addr.sin_addr.S_un.S_addr) {
		// TODO: Decrypt buf
		memcpy(msg, buf, sizeof(msg));
	}
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
	local_address.sin_port = htons(9999);
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

