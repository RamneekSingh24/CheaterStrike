#pragma once
#include "Enclave_u.h"

#include <iostream>
#include <Windows.h>
#include "sgx_urts.h"
#include "sgx_tcrypto.h"
#include "sgx_uae_service.h"

class EnclaveWrapper
{

public:
	int init(unsigned char*);
	int deriveSharedKey(unsigned char* peer_pub_key);
	int deriveAuthKey(unsigned char* peer_pub_key);
	int verifyInput(unsigned char* input_with_auth_tag);
	int decryptRecord(unsigned char* in_record, uint16_t in_len, unsigned char* out_msg, uint16_t out_len);
	unsigned char enclavePublicKey[64];

private:
	sgx_enclave_id_t eid;
};

