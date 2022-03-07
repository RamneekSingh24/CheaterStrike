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


private:
	sgx_enclave_id_t eid;
};

