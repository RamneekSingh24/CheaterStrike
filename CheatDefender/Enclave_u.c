#include "Enclave_u.h"
#include <errno.h>

typedef struct ms_gen_keys_t {
	unsigned char* ms_pub_key;
} ms_gen_keys_t;

typedef struct ms_gen_secret_t {
	unsigned char* ms_remote_pub_key;
	unsigned char* ms_shared_key;
} ms_gen_secret_t;

typedef struct ms_authenticate_input_t {
	unsigned char* ms_input_with_auth_tag;
	unsigned char* ms_res;
} ms_authenticate_input_t;

typedef struct ms_gen_auth_key_t {
	unsigned char* ms_remote_pub_key;
	unsigned char* ms_shared_key;
} ms_gen_auth_key_t;

typedef struct ms_get_msg_from_record_t {
	unsigned char* ms_in_record;
	uint16_t ms_in_len;
	unsigned char* ms_out_msg;
	uint16_t ms_out_len;
} ms_get_msg_from_record_t;

static const struct {
	size_t nr_ocall;
	void * func_addr[1];
} ocall_table_Enclave = {
	0,
	{ NULL },
};

sgx_status_t init_ecc(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 0, &ocall_table_Enclave, NULL);
	return status;
}

sgx_status_t gen_keys(sgx_enclave_id_t eid, unsigned char* pub_key)
{
	sgx_status_t status;
	ms_gen_keys_t ms;
	ms.ms_pub_key = pub_key;
	status = sgx_ecall(eid, 1, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t gen_secret(sgx_enclave_id_t eid, unsigned char* remote_pub_key, unsigned char* shared_key)
{
	sgx_status_t status;
	ms_gen_secret_t ms;
	ms.ms_remote_pub_key = remote_pub_key;
	ms.ms_shared_key = shared_key;
	status = sgx_ecall(eid, 2, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t authenticate_input(sgx_enclave_id_t eid, unsigned char* input_with_auth_tag, unsigned char* res)
{
	sgx_status_t status;
	ms_authenticate_input_t ms;
	ms.ms_input_with_auth_tag = input_with_auth_tag;
	ms.ms_res = res;
	status = sgx_ecall(eid, 3, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t gen_auth_key(sgx_enclave_id_t eid, unsigned char* remote_pub_key, unsigned char* shared_key)
{
	sgx_status_t status;
	ms_gen_auth_key_t ms;
	ms.ms_remote_pub_key = remote_pub_key;
	ms.ms_shared_key = shared_key;
	status = sgx_ecall(eid, 4, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t get_msg_from_record(sgx_enclave_id_t eid, unsigned char* in_record, uint16_t in_len, unsigned char* out_msg, uint16_t out_len)
{
	sgx_status_t status;
	ms_get_msg_from_record_t ms;
	ms.ms_in_record = in_record;
	ms.ms_in_len = in_len;
	ms.ms_out_msg = out_msg;
	ms.ms_out_len = out_len;
	status = sgx_ecall(eid, 5, &ocall_table_Enclave, &ms);
	return status;
}

