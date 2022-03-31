#include "Enclave_t.h"
#include "sgx_trts.h"
#include "sgx_tcrypto.h"
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <string.h>
#include "aes.h"
#include "cmac.h"
#include "../CheatDefender/game/Record.h"
#include "../CheatDefender/game/Message.h"

sgx_ecc_state_handle_t ecc_context;
sgx_ec256_private_t p_private;
sgx_ec256_public_t p_public;
sgx_ec256_dh_shared_t p_shared;
AES_ctx aes_ctx;


sgx_ec256_dh_shared_t p_shared_auth;

unsigned char auth_key[16];

void init_ecc() {
	sgx_ecc256_open_context(&ecc_context);

}

void gen_keys(unsigned char* pub_key) {
	sgx_ecc256_create_key_pair(&p_private, &p_public,
		ecc_context);
	memcpy(pub_key, &p_public, 64);
}

void gen_secret(unsigned char* remote_pub_key, unsigned char* shared_key) {

	sgx_ecc256_compute_shared_dhkey(
		&p_private,
		reinterpret_cast<sgx_ec256_public_t*> (remote_pub_key),
		&p_shared,
		ecc_context
	);
	memcpy(shared_key, &p_shared, 32);


	AES_init_ctx_iv(&aes_ctx,
		reinterpret_cast<uint8_t*>(shared_key), // Aes 128 bit key
		reinterpret_cast<uint8_t*>(shared_key) + 16 // 128 bit Iv
	);


}

void gen_auth_key(unsigned char* remote_pub_key,  unsigned char* shared_key) {
	
	sgx_ecc256_compute_shared_dhkey(
		&p_private,
		reinterpret_cast<sgx_ec256_public_t*> (remote_pub_key),
		&p_shared_auth,
		ecc_context
	);
	memcpy(shared_key, &p_shared_auth, 16);

}


void authenticate_input(unsigned char* input_with_auth_tag, unsigned char* res) {
	bool verified = cmac::verify_mac(input_with_auth_tag, 8, (input_with_auth_tag + 8), auth_key);
	if (verified) *res = 1;
	else *res = 0;
}





bool isActualPlayer(PlayerState* player) {
	if (player->x == -100.0f && player->y == -100.0f && player->z == -100.0f && player->clientID == UNKNOWN_ID)
		return false;
	else
		return true;
}

bool isNotBehindWall(PlayerState* player) {
	if (player->z < 0.3f && player->x < 3.0f && player->x > -3.0f) {
		return false;
	}
	else {
		return true;
	}
}

void get_msg_from_record(unsigned char* in_record, uint16_t in_len, unsigned char* out_msg, uint16_t out_len) {

	// Do filtering of sensitive data here


	Record* record = reinterpret_cast<Record*> (in_record);
	Message* msg = reinterpret_cast<Message*> (out_msg);

	memcpy(msg, &record->msg, sizeof(Message));

	assert(record->encrypted == ENCRYPTED);
	assert(in_len == sizeof(Record));
	assert(out_len == sizeof(Message));

	AES_CTR_xcrypt_buffer(&aes_ctx,
		reinterpret_cast<uint8_t*>(msg),
		sizeof(Message),
		record->seq_no
	);
	int visiblePlayersIndex[MAX_PLAYERS];
	int counter = 0;
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (isActualPlayer(&msg->state[i]) && isNotBehindWall(&msg->state[i])) {
			visiblePlayersIndex[counter] = i;
			counter++;
		}
	}

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (i < counter) {
			//msg->state[i] = msg->state[visiblePlayersIndex[i]];
			msg->state[i].x = msg->state[visiblePlayersIndex[i]].x;
			msg->state[i].y = msg->state[visiblePlayersIndex[i]].y;
			msg->state[i].z = msg->state[visiblePlayersIndex[i]].z;

			msg->state[i].clientID = msg->state[visiblePlayersIndex[i]].clientID;

			for (int j = 0; j < NUM_KEYS; j++) {
				msg->state[i].last_input[j] = msg->state[visiblePlayersIndex[i]].last_input[j];
			}

		}
		else {
			msg->state[i].x = -100.0f;
			msg->state[i].y = -100.0f;
			msg->state[i].z = -100.0f;


			for (int j = 0; j < NUM_KEYS; j++) {
				msg->state[i].last_input[j] = false;
			}
		}

	}


}