#ifndef ENCLAVE_T_H__
#define ENCLAVE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */


#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

void init_ecc(void);
void gen_keys(unsigned char* pub_key);
void gen_secret(unsigned char* remote_pub_key, unsigned char* shared_key);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
