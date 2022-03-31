#pragma once
#include <stdint.h>
#include <string.h>


#define AES_BLOCKLEN 16 

#define AES_KEYLEN 16   
#define AES_keyExpSize 176


struct AES_ctx
{
	uint8_t RoundKey[AES_keyExpSize];

	uint8_t Iv[AES_BLOCKLEN];

};

void AES_init_ctx(struct AES_ctx* ctx, const uint8_t* key);

void AES_init_ctx_iv(struct AES_ctx* ctx, const uint8_t* key, const uint8_t* iv);
void AES_ctx_set_iv(struct AES_ctx* ctx, const uint8_t* iv);

void AES_CTR_xcrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, uint32_t length, uint32_t base_block_no);

