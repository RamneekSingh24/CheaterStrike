#include "EnclaveWrapper.h"
#include <iostream>
#include <iomanip>

int EnclaveWrapper::init(unsigned char* pub_key)
{
	memcpy(enclavePublicKey, pub_key, 64);
	sgx_enclave_id_t   eid;
	sgx_status_t       ret = SGX_SUCCESS;
	sgx_launch_token_t token = { 0 };
	const char* ENCLAVE_FILE_NAME = "C:/Users/ramne/Desktop/sil-project/CheatDefender/CheatDefender/Debug/Enclave.signed.dll";
	int updated = 0;
	int res = -1;

	//Check if SGX can be enabled and activate enclave
	sgx_device_status_t sgx_device_status;
	ret = sgx_enable_device(&sgx_device_status);


	if (ret != SGX_SUCCESS) {
		return ret;
	}

	ret = sgx_create_enclave(ENCLAVE_FILE_NAME, SGX_DEBUG_FLAG, &token, &updated, &eid, NULL);


	if (ret != SGX_SUCCESS) {
		return ret;
	} 

	ret = init_ecc(eid);

	if (ret != SGX_SUCCESS) {
		return ret;
	}

	ret = gen_keys(eid, pub_key);

	this->eid = eid;
	return ret;

}

int EnclaveWrapper::deriveSharedKey(unsigned char* peer_pub_key)
{
	sgx_status_t ret = SGX_SUCCESS;
	unsigned char shared_key[32] = { 0 };		// TODO: Remove this later 


	std::cout << "[Client] peer pub Key:\n";
	std::cout << std::hex;
	for (int i = 0; i < 64; i++) {
		std::cout << std::setfill('0') << std::setw(2) << (int)peer_pub_key[i] << " ";
	}
	std::cout << std::dec << "\n";



	ret = gen_secret(eid, peer_pub_key, shared_key);

	std::cout << "[Client] Derived Session Key:\n";
	std::cout << std::hex;
	for (int i = 0; i < 32; i++) {
		std::cout << std::setfill('0') << std::setw(2) << (int) shared_key[i] << " ";
	}
	std::cout << std::dec << "\n";

	return ret;

}


int EnclaveWrapper::deriveAuthKey(unsigned char* peer_pub_key)
{
	sgx_status_t ret = SGX_SUCCESS;
	unsigned char shared_key[16] = { 1 };   // TODO: Remove this later 
	ret = gen_auth_key(eid, peer_pub_key, shared_key);

	std::cout << "[Client] Derived Auth Key:\n";
	std::cout << std::hex;
	for (int i = 0; i < 16; i++) {
		std::cout << std::setfill('0') << std::setw(2) << (int)shared_key[i] << " ";
	}
	std::cout << std::dec << "\n";

	return ret;

}

int EnclaveWrapper::verifyInput(unsigned char* input_with_auth_tag)
{
	unsigned char out = 0;
	authenticate_input(eid, input_with_auth_tag, &out);
	if (out == 0) {
		std::cout << "[Client Warning] Aimbot detected by enclave, you will be booted out of server soon" << std::endl;
	}

	return 0;
}

int EnclaveWrapper::decryptRecord(unsigned char* in_record, uint16_t in_len, unsigned char* out_msg, uint16_t out_len)
{


	sgx_status_t ret = SGX_SUCCESS;

	ret =  get_msg_from_record(eid, in_record, in_len, out_msg, out_len);

	return ret;
}





