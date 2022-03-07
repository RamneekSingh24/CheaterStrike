#include "Network.h"
#include "KeyExchange.h"
#include <iostream>

std::string serverIP = "127.0.0.1";
int serverPort = 8866;

PlayerState players[MAX_PLAYERS];
KeyExchange keyExchange[MAX_PLAYERS];

int num_players_joined = 0;


int main() {

	Network net;
	net.init(serverIP, serverPort);

	int bytes_recv;
	Message msg_recv, msg_send;
	std::string recv_ip;
	int recv_port;

	while (true) {
		bytes_recv = net.recv(&msg_recv, recv_ip, recv_port);
		if (bytes_recv <= 0) continue;
		switch (msg_recv.msgType) {
			case CL_SV_MSG_JOIN:
				msg_send.clientId = num_players_joined++;
				msg_send.msgType = SV_CL_MSG_JOIN_OK;
				std::cout << "player joined " << recv_ip << " " << recv_port << std::endl;
				net.send(msg_send, recv_ip, recv_port);
				break;
			case CL_SV_MSG_QUIT:
				players[msg_recv.clientId].x = -100;
				players[msg_recv.clientId].y = -100;
				players[msg_recv.clientId].z = -100;
				std::cout << "player quit\n";
				break;
			case CL_SV_MSG_LOCAL_STATE:
				players[msg_recv.clientId] = msg_recv.state[msg_recv.clientId];
				msg_send.msgType = SV_CL_MSG_ALL_STATE;
				msg_send.clientId = msg_recv.clientId;
				for (int i = 0; i < MAX_PLAYERS; i++) {
					msg_send.state[i] = players[i];
				}
				net.send(msg_send, recv_ip, recv_port);
				break;
			case KEY_EXCHANGE:
				KeyExchange& keyEx = keyExchange[msg_recv.clientId];
				keyEx.init();
				keyEx.generate_shared_key(msg_recv.ecc_pub_key);
				msg_send.msgType = KEY_EXCHANGE;
				memcpy(msg_send.ecc_pub_key, keyEx.get_public_key(), sizeof(sample_ec256_public_t));
				msg_send.clientId = msg_recv.clientId;
				net.send(msg_send, recv_ip, recv_port);

		}

	}


	
}