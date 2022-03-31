#include "Network.h"
#include <iostream>

int Network::send(Message msg, std::string clientIP, int clientPort, unsigned char* shared_secret)
{

	sockaddr_in recvr_addr;

	recvr_addr.sin_family = AF_INET;
	recvr_addr.sin_port = htons(clientPort);
	recvr_addr.sin_addr.S_un.S_addr = inet_addr(clientIP.c_str());

	std::string addr = clientIP + std::to_string(clientPort);

	Record record;
	record.seq_no = ++send_seq_no[clientIP + std::to_string(clientPort)];   // TODO: Can correctly assign seq_no 
	record.msg = msg;														// according to AES_BLOCK_SIZE
																			// But this works for now.
				

	
	if (msg.msgType == SV_CL_MSG_ALL_STATE) {   // Only need to encrypt the all state message. 
		record.encrypted = encryptionOn ? ENCRYPTED : NOT_ENCRYPTED;

		unsigned char* iv = (shared_secret + 16);

		// Init Aes context if not present
		if (encryptionOn) {
			if (aes_ctxs.find(addr) == aes_ctxs.end()) {
				AES_init_ctx_iv(&aes_ctxs[addr],
					reinterpret_cast<uint8_t*>(shared_secret),
					reinterpret_cast<uint8_t*>(iv));
			}

			// Encrypt Message

			AES_CTR_xcrypt_buffer(&aes_ctxs[addr],
				reinterpret_cast<uint8_t*>(&record.msg),
				sizeof(Message),
				record.seq_no);
		}
	}
	else {
		record.encrypted = NOT_ENCRYPTED;
	}


	char message[BUFLEN];
	memcpy(message, &record, sizeof(Record));

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

	memcpy(msg, buf, sizeof(msg));
	char fromIP[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(from.sin_addr), fromIP, INET_ADDRSTRLEN);
	clientIP = std::string(fromIP);
	clientPort = ntohs(from.sin_port);

	return bytes_received;
}

int Network::init(std::string myIP, int myPort, bool encryption)
{


	
	encryptionOn = encryption;


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




