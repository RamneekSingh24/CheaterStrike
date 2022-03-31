#include "Enclave_t.h"

#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */
#include "sgx_lfence.h" /* for sgx_lfence */

#include <errno.h>
#include <mbusafecrt.h> /* for memcpy_s etc */
#include <stdlib.h> /* for malloc/free etc */

#define CHECK_REF_POINTER(ptr, siz) do {	\
	if (!(ptr) || ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_UNIQUE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_ENCLAVE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_within_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define ADD_ASSIGN_OVERFLOW(a, b) (	\
	((a) += (b)) < (b)	\
)


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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4200)
#endif

static sgx_status_t SGX_CDECL sgx_init_ecc(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	init_ecc();
	return status;
}

static sgx_status_t SGX_CDECL sgx_gen_keys(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_gen_keys_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_gen_keys_t* ms = SGX_CAST(ms_gen_keys_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	unsigned char* _tmp_pub_key = ms->ms_pub_key;
	size_t _len_pub_key = 64 * sizeof(unsigned char);
	unsigned char* _in_pub_key = NULL;

	if (sizeof(*_tmp_pub_key) != 0 &&
		64 > (SIZE_MAX / sizeof(*_tmp_pub_key))) {
		return SGX_ERROR_INVALID_PARAMETER;
	}

	CHECK_UNIQUE_POINTER(_tmp_pub_key, _len_pub_key);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_pub_key != NULL && _len_pub_key != 0) {
		if ( _len_pub_key % sizeof(*_tmp_pub_key) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		if ((_in_pub_key = (unsigned char*)malloc(_len_pub_key)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_pub_key, 0, _len_pub_key);
	}

	gen_keys(_in_pub_key);
	if (_in_pub_key) {
		if (memcpy_s(_tmp_pub_key, _len_pub_key, _in_pub_key, _len_pub_key)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

err:
	if (_in_pub_key) free(_in_pub_key);
	return status;
}

static sgx_status_t SGX_CDECL sgx_gen_secret(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_gen_secret_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_gen_secret_t* ms = SGX_CAST(ms_gen_secret_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	unsigned char* _tmp_remote_pub_key = ms->ms_remote_pub_key;
	size_t _len_remote_pub_key = 64 * sizeof(unsigned char);
	unsigned char* _in_remote_pub_key = NULL;
	unsigned char* _tmp_shared_key = ms->ms_shared_key;
	size_t _len_shared_key = 32 * sizeof(unsigned char);
	unsigned char* _in_shared_key = NULL;

	if (sizeof(*_tmp_remote_pub_key) != 0 &&
		64 > (SIZE_MAX / sizeof(*_tmp_remote_pub_key))) {
		return SGX_ERROR_INVALID_PARAMETER;
	}

	if (sizeof(*_tmp_shared_key) != 0 &&
		32 > (SIZE_MAX / sizeof(*_tmp_shared_key))) {
		return SGX_ERROR_INVALID_PARAMETER;
	}

	CHECK_UNIQUE_POINTER(_tmp_remote_pub_key, _len_remote_pub_key);
	CHECK_UNIQUE_POINTER(_tmp_shared_key, _len_shared_key);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_remote_pub_key != NULL && _len_remote_pub_key != 0) {
		if ( _len_remote_pub_key % sizeof(*_tmp_remote_pub_key) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		_in_remote_pub_key = (unsigned char*)malloc(_len_remote_pub_key);
		if (_in_remote_pub_key == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_remote_pub_key, _len_remote_pub_key, _tmp_remote_pub_key, _len_remote_pub_key)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

	}
	if (_tmp_shared_key != NULL && _len_shared_key != 0) {
		if ( _len_shared_key % sizeof(*_tmp_shared_key) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		if ((_in_shared_key = (unsigned char*)malloc(_len_shared_key)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_shared_key, 0, _len_shared_key);
	}

	gen_secret(_in_remote_pub_key, _in_shared_key);
	if (_in_shared_key) {
		if (memcpy_s(_tmp_shared_key, _len_shared_key, _in_shared_key, _len_shared_key)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

err:
	if (_in_remote_pub_key) free(_in_remote_pub_key);
	if (_in_shared_key) free(_in_shared_key);
	return status;
}

static sgx_status_t SGX_CDECL sgx_authenticate_input(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_authenticate_input_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_authenticate_input_t* ms = SGX_CAST(ms_authenticate_input_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	unsigned char* _tmp_input_with_auth_tag = ms->ms_input_with_auth_tag;
	size_t _len_input_with_auth_tag = 24 * sizeof(unsigned char);
	unsigned char* _in_input_with_auth_tag = NULL;
	unsigned char* _tmp_res = ms->ms_res;
	size_t _len_res = 1 * sizeof(unsigned char);
	unsigned char* _in_res = NULL;

	if (sizeof(*_tmp_input_with_auth_tag) != 0 &&
		24 > (SIZE_MAX / sizeof(*_tmp_input_with_auth_tag))) {
		return SGX_ERROR_INVALID_PARAMETER;
	}

	if (sizeof(*_tmp_res) != 0 &&
		1 > (SIZE_MAX / sizeof(*_tmp_res))) {
		return SGX_ERROR_INVALID_PARAMETER;
	}

	CHECK_UNIQUE_POINTER(_tmp_input_with_auth_tag, _len_input_with_auth_tag);
	CHECK_UNIQUE_POINTER(_tmp_res, _len_res);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_input_with_auth_tag != NULL && _len_input_with_auth_tag != 0) {
		if ( _len_input_with_auth_tag % sizeof(*_tmp_input_with_auth_tag) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		_in_input_with_auth_tag = (unsigned char*)malloc(_len_input_with_auth_tag);
		if (_in_input_with_auth_tag == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_input_with_auth_tag, _len_input_with_auth_tag, _tmp_input_with_auth_tag, _len_input_with_auth_tag)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

	}
	if (_tmp_res != NULL && _len_res != 0) {
		if ( _len_res % sizeof(*_tmp_res) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		if ((_in_res = (unsigned char*)malloc(_len_res)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_res, 0, _len_res);
	}

	authenticate_input(_in_input_with_auth_tag, _in_res);
	if (_in_res) {
		if (memcpy_s(_tmp_res, _len_res, _in_res, _len_res)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

err:
	if (_in_input_with_auth_tag) free(_in_input_with_auth_tag);
	if (_in_res) free(_in_res);
	return status;
}

static sgx_status_t SGX_CDECL sgx_gen_auth_key(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_gen_auth_key_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_gen_auth_key_t* ms = SGX_CAST(ms_gen_auth_key_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	unsigned char* _tmp_remote_pub_key = ms->ms_remote_pub_key;
	size_t _len_remote_pub_key = 64 * sizeof(unsigned char);
	unsigned char* _in_remote_pub_key = NULL;
	unsigned char* _tmp_shared_key = ms->ms_shared_key;
	size_t _len_shared_key = 16 * sizeof(unsigned char);
	unsigned char* _in_shared_key = NULL;

	if (sizeof(*_tmp_remote_pub_key) != 0 &&
		64 > (SIZE_MAX / sizeof(*_tmp_remote_pub_key))) {
		return SGX_ERROR_INVALID_PARAMETER;
	}

	if (sizeof(*_tmp_shared_key) != 0 &&
		16 > (SIZE_MAX / sizeof(*_tmp_shared_key))) {
		return SGX_ERROR_INVALID_PARAMETER;
	}

	CHECK_UNIQUE_POINTER(_tmp_remote_pub_key, _len_remote_pub_key);
	CHECK_UNIQUE_POINTER(_tmp_shared_key, _len_shared_key);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_remote_pub_key != NULL && _len_remote_pub_key != 0) {
		if ( _len_remote_pub_key % sizeof(*_tmp_remote_pub_key) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		_in_remote_pub_key = (unsigned char*)malloc(_len_remote_pub_key);
		if (_in_remote_pub_key == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_remote_pub_key, _len_remote_pub_key, _tmp_remote_pub_key, _len_remote_pub_key)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

	}
	if (_tmp_shared_key != NULL && _len_shared_key != 0) {
		if ( _len_shared_key % sizeof(*_tmp_shared_key) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		if ((_in_shared_key = (unsigned char*)malloc(_len_shared_key)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_shared_key, 0, _len_shared_key);
	}

	gen_auth_key(_in_remote_pub_key, _in_shared_key);
	if (_in_shared_key) {
		if (memcpy_s(_tmp_shared_key, _len_shared_key, _in_shared_key, _len_shared_key)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

err:
	if (_in_remote_pub_key) free(_in_remote_pub_key);
	if (_in_shared_key) free(_in_shared_key);
	return status;
}

static sgx_status_t SGX_CDECL sgx_get_msg_from_record(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_get_msg_from_record_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_get_msg_from_record_t* ms = SGX_CAST(ms_get_msg_from_record_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	unsigned char* _tmp_in_record = ms->ms_in_record;
	uint16_t _tmp_in_len = ms->ms_in_len;
	size_t _len_in_record = _tmp_in_len * sizeof(unsigned char);
	unsigned char* _in_in_record = NULL;
	unsigned char* _tmp_out_msg = ms->ms_out_msg;
	uint16_t _tmp_out_len = ms->ms_out_len;
	size_t _len_out_msg = _tmp_out_len * sizeof(unsigned char);
	unsigned char* _in_out_msg = NULL;

	if (sizeof(*_tmp_in_record) != 0 &&
		(size_t)_tmp_in_len > (SIZE_MAX / sizeof(*_tmp_in_record))) {
		return SGX_ERROR_INVALID_PARAMETER;
	}

	if (sizeof(*_tmp_out_msg) != 0 &&
		(size_t)_tmp_out_len > (SIZE_MAX / sizeof(*_tmp_out_msg))) {
		return SGX_ERROR_INVALID_PARAMETER;
	}

	CHECK_UNIQUE_POINTER(_tmp_in_record, _len_in_record);
	CHECK_UNIQUE_POINTER(_tmp_out_msg, _len_out_msg);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_in_record != NULL && _len_in_record != 0) {
		if ( _len_in_record % sizeof(*_tmp_in_record) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		_in_in_record = (unsigned char*)malloc(_len_in_record);
		if (_in_in_record == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_in_record, _len_in_record, _tmp_in_record, _len_in_record)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

	}
	if (_tmp_out_msg != NULL && _len_out_msg != 0) {
		if ( _len_out_msg % sizeof(*_tmp_out_msg) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		if ((_in_out_msg = (unsigned char*)malloc(_len_out_msg)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_out_msg, 0, _len_out_msg);
	}

	get_msg_from_record(_in_in_record, _tmp_in_len, _in_out_msg, _tmp_out_len);
	if (_in_out_msg) {
		if (memcpy_s(_tmp_out_msg, _len_out_msg, _in_out_msg, _len_out_msg)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

err:
	if (_in_in_record) free(_in_in_record);
	if (_in_out_msg) free(_in_out_msg);
	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* call_addr; uint8_t is_priv; uint8_t is_switchless;} ecall_table[6];
} g_ecall_table = {
	6,
	{
		{(void*)(uintptr_t)sgx_init_ecc, 0, 0},
		{(void*)(uintptr_t)sgx_gen_keys, 0, 0},
		{(void*)(uintptr_t)sgx_gen_secret, 0, 0},
		{(void*)(uintptr_t)sgx_authenticate_input, 0, 0},
		{(void*)(uintptr_t)sgx_gen_auth_key, 0, 0},
		{(void*)(uintptr_t)sgx_get_msg_from_record, 0, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
} g_dyn_entry_table = {
	0,
};


#ifdef _MSC_VER
#pragma warning(pop)
#endif
