#include <stdlib.h>
#include <string.h>
#include <assert.h>

namespace cmac {


    static unsigned char sbox[] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
    };


    static unsigned char Rcon[] = {
    0b00000000, 0x00, 0x00, 0x00,
    0b00000001, 0x00, 0x00, 0x00,
    0b00000010, 0x00, 0x00, 0x00,
    0b00000100, 0x00, 0x00, 0x00,
    0b00001000, 0x00, 0x00, 0x00,
    0b00010000, 0x00, 0x00, 0x00,
    0b00100000, 0x00, 0x00, 0x00,
    0b01000000, 0x00, 0x00, 0x00,
    0b10000000, 0x00, 0x00, 0x00,
    0x1b, 0x00, 0x00, 0x00,
    0x36, 0x00, 0x00, 0x00
    };

    const int const_Bsize = 16;


    unsigned char* SubWord(unsigned char* word)
    {
        for (auto i = 0; i < 4; i++) {
            word[i] = sbox[word[i]];
        }
        return word;
    }


    unsigned char* RotWord(unsigned char* word)
    {
        unsigned char tmp[4];
        memcpy(tmp, word, 4);
        for (auto i = 0; i < 4; i++) {
            word[i] = tmp[(i + 1) % 4];
        }
        return word;
    }


    void KeyExpansion(unsigned char* key, unsigned char* w, unsigned char Nk, unsigned char Nr)
    {
        unsigned char tmp[4];
        memcpy(w, key, 4 * Nk);

        for (auto i = 4 * Nk; i < 4 * (Nr + 1) * 4; i += 4) {
            memcpy(tmp, w + i - 4, 4);
            if (i % (Nk * 4) == 0) {
                SubWord(RotWord(tmp));
                for (auto j = 0; j < 4; j++) {
                    tmp[j] ^= Rcon[i / Nk + j];
                }
            }
            else if (Nk > 6 && (i % (Nk * 4)) == 16) {
                SubWord(tmp);
            }
            for (auto j = 0; j < 4; j++)
                w[i + j] = w[i - Nk * 4 + j] ^ tmp[j];
        }
    }

    void AddRoundKey(unsigned char state[4][4], unsigned char* key)
    {
        for (auto row = 0; row < 4; row++) {
            for (auto col = 0; col < 4; col++) {
                state[col][row] ^= key[col * 4 + row];
            }
        }
    }



    void SubBytes(unsigned char state[4][4])
    {
        for (auto row = 0; row < 4; row++) {
            for (auto col = 0; col < 4; col++) {
                state[col][row] = sbox[state[col][row]];
            }
        }
    }


    void ShiftRows(unsigned char state[4][4])
    {
        unsigned char tmp[4];
        for (auto row = 1; row < 4; row++) {
            for (auto col = 0; col < 4; col++) {
                tmp[col] = state[(row + col) % 4][row];
            }
            for (auto col = 0; col < 4; col++) {
                state[col][row] = tmp[col];
            }
        }
    }


    unsigned char mul(unsigned char a, unsigned char b)
    {
        unsigned char sb[4];
        unsigned char out = 0;
        sb[0] = b;
        for (auto i = 1; i < 4; i++) {
            sb[i] = sb[i - 1] << 1;
            if (sb[i - 1] & 0x80) {
                sb[i] ^= 0x1b;
            }
        }
        for (auto i = 0; i < 4; i++) {
            if (a >> i & 0x01) {
                out ^= sb[i];
            }
        }
        return out;
    }

    void MixColumns(unsigned char state[4][4])
    {
        unsigned char tmp[4];
        unsigned char matmul[][4] = {
            0x02, 0x03, 0x01, 0x01,
            0x01, 0x02, 0x03, 0x01,
            0x01, 0x01, 0x02, 0x03,
            0x03, 0x01, 0x01, 0x02
        };
        for (auto col = 0; col < 4; col++) {
            for (auto row = 0; row < 4; row++) {
                tmp[row] = state[col][row];
            }
            for (auto i = 0; i < 4; i++) {
                state[col][i] = 0x00;
                for (auto j = 0; j < 4; j++) {
                    state[col][i] ^= mul(matmul[i][j], tmp[j]);
                }
            }
        }
    }



    void Cipher(unsigned char* in, unsigned char* out, unsigned char* w, unsigned char Nk, unsigned char Nr)
    {
        unsigned char state[4][4];
        assert(Nk == 4);
        memcpy(state, in, 4 * Nk);

        AddRoundKey(state, w);
        for (auto round = 0; round < Nr; round++) {
            SubBytes(state);
            ShiftRows(state);
            if (round != (Nr - 1))
                MixColumns(state);
            AddRoundKey(state, (unsigned char*)(w + (round + 1) * 16));
        }
        memcpy(out, state, Nk * 4);
    }


    unsigned char* aes_128_encrypt(unsigned char* in, unsigned char* out, unsigned char* key)
    {
        unsigned char* w;
        unsigned char Nk = 4, Nr = 10;
        w = (unsigned char*)malloc(16 * (Nr + 1));
        KeyExpansion(key, w, Nk, Nr);
        Cipher(in, out, w, Nk, Nr);
        free(w);
        return out;
    }

    void block_xor(unsigned char* dst, unsigned char* a, unsigned char* b)
    {
        for (auto j = 0; j < 16; j++) {
            dst[j] = a[j] ^ b[j];
        }
    }


    void block_leftshift(unsigned char* dst, unsigned char* src)
    {
        unsigned char ovf = 0x00;
        for (auto i = 15; i >= 0; i--) {
            dst[i] = src[i] << 1;
            dst[i] |= ovf;
            ovf = (src[i] & 0x80) ? 1 : 0;
        }
    }



    // Generate the Sub keys
    void GenerateSubkey(unsigned char* key, unsigned char* K1, unsigned char* K2)
    {
        unsigned char const_Zero[] = {
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
        };

        unsigned char const_Rb[] = {
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x87
        };

        unsigned char L[16];
        aes_128_encrypt(const_Zero, L, key);
        block_leftshift(K1, L);
        if (L[0] & 0x80) {
            block_xor(K1, K1, const_Rb);
        }

        block_leftshift(K2, K1);
        if (K1[0] & 0x80) {
            block_xor(K2, K2, const_Rb);
        }
    }

    // Calculate the CAMC
    unsigned char* aes_cmac(unsigned char* in, unsigned int length, unsigned char* out, unsigned char* key)
    {
        unsigned char K1[16];
        unsigned char K2[16];

        GenerateSubkey(key, K1, K2);

        int n = (length / const_Bsize);
        bool flag = false;
        if (length % const_Bsize != 0) {
            n++;
        }

        if (n == 0) {
            n = 1;
        }
        else if (length % const_Bsize == 0) {
            flag = true;
        }

        
        unsigned char** M = (unsigned char**)malloc(n * sizeof(unsigned char*));
        for (int i = 0; i < n; i++) {
            M[i] = (unsigned char*)malloc(const_Bsize * sizeof(unsigned char));
        }

        memset(M[0], 0, n * const_Bsize);
        memcpy(M[0], in, length);
        if (!flag) {
            memset(M[0] + length, 0x80, 1);
        }
        if (flag) {
            block_xor(M[n - 1], M[n - 1], K1);
        }
        else {
            block_xor(M[n - 1], M[n - 1], K2);
        }

        unsigned char X[] = {
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
        };
        unsigned char Y[const_Bsize];

        for (auto i = 0; i < n - 1; i++) {
            block_xor(Y, M[i], X);
            aes_128_encrypt(Y, X, key);
        }
        block_xor(Y, M[n - 1], X);
        aes_128_encrypt(Y, out, key);


        for (int i = 0; i < n; i++) {
            free(M[i]);
        }
        free(M);



        return out;
    }

    // Verify the CMAC
    bool verify_mac(unsigned char* in, unsigned int length, unsigned char* out, unsigned char* key)
    {
        bool flag = true;
        unsigned char result[16];
        aes_cmac(in, length, (unsigned char*)result, key);
        for (auto i = 0; i < const_Bsize; i++) {
            if (!(result[i] ^ out[i])) {
                flag = false;
                break;
            }
        }
        return flag;
    }


}