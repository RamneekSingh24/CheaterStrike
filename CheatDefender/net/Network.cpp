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

	Record record;
	memcpy(&record, buffer, sizeof(Record));


	if (record.encrypted == NOT_ENCRYPTED) {
		memcpy(msg, &record.msg, sizeof(Message));
	}
	
	else {
			int ret = enc_w->decryptRecord(
			reinterpret_cast<unsigned char*> (&record), sizeof(Record),
			reinterpret_cast<unsigned char*> (msg), sizeof(Message));
			if (ret != 0) {
				return ret;
			}
	}


	return bytes_received;
}

int Network::init(std::string serverIP, int serverPort, EnclaveWrapper* enc_w)
{



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

	this->enc_w = enc_w;


	return 0;
}
