#pragma once
#include "sample_libcrypto.h"
#include "Network.h"
class KeyExchange
{
public:
	KeyExchange() {};

	int init();
	int generate_shared_key(unsigned char* peer_pub_key);
	unsigned char* get_public_key();

	unsigned char session_key[SAMPLE_ECP256_KEY_SIZE];



private:
	sample_ecc_state_handle_t ecc_handle;
	sample_ec256_private_t p_private;
	sample_ec256_public_t p_public;
	sample_ec256_dh_shared_t p_shared_key;
	Network* net;
};

