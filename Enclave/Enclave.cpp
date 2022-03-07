#include "Enclave_t.h"
#include "sgx_trts.h"
#include "sgx_tcrypto.h"
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <string.h>


sgx_ecc_state_handle_t ecc_context;
sgx_ec256_private_t p_private;
sgx_ec256_public_t p_public;
sgx_ec256_dh_shared_t p_shared;

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
}



