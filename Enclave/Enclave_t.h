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
void authenticate_input(unsigned char* input_with_auth_tag, unsigned char* res);
void gen_auth_key(unsigned char* remote_pub_key, unsigned char* shared_key);
void get_msg_from_record(unsigned char* in_record, uint16_t in_len, unsigned char* out_msg, uint16_t out_len);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
