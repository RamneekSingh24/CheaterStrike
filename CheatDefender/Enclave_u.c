#include "Enclave_u.h"
#include <errno.h>

typedef struct ms_gen_keys_t {
	unsigned char* ms_pub_key;
} ms_gen_keys_t;

typedef struct ms_gen_secret_t {
	unsigned char* ms_remote_pub_key;
	unsigned char* ms_shared_key;
} ms_gen_secret_t;

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

