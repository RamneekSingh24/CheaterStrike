#ifndef ENCLAVE_U_H__
#define ENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_status_t etc. */


#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif


sgx_status_t init_ecc(sgx_enclave_id_t eid);
sgx_status_t gen_keys(sgx_enclave_id_t eid, unsigned char* pub_key);
sgx_status_t gen_secret(sgx_enclave_id_t eid, unsigned char* remote_pub_key, unsigned char* shared_key);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
