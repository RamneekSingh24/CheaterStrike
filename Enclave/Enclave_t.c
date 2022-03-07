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

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* call_addr; uint8_t is_priv; uint8_t is_switchless;} ecall_table[3];
} g_ecall_table = {
	3,
	{
		{(void*)(uintptr_t)sgx_init_ecc, 0, 0},
		{(void*)(uintptr_t)sgx_gen_keys, 0, 0},
		{(void*)(uintptr_t)sgx_gen_secret, 0, 0},
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
