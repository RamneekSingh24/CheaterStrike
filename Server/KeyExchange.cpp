#include "KeyExchange.h"
#include <iostream>
#include <iomanip>

int KeyExchange::init()
{


    sample_status_t res;
    res = sample_ecc256_open_context(&ecc_handle);
    if (res != SAMPLE_SUCCESS) {
        return res;
    }

    res = sample_ecc256_create_key_pair(&p_private, &p_public, ecc_handle);

    return res;
}

int KeyExchange::generate_shared_key(unsigned char* peer_pub_key)
{   

    sample_status_t res;
    res = sample_ecc256_compute_shared_dhkey(&p_private,
    reinterpret_cast<sample_ec256_public_t*> (peer_pub_key), &p_shared_key, ecc_handle);
    memcpy(this->session_key, &p_shared_key, SAMPLE_ECP256_KEY_SIZE);


    std::cout << "[Server] Derived Session Key:\n";
    std::cout << std::hex;
    for (int i = 0; i < 32; i++) {
        std::cout << std::setfill('0') << std::setw(2) << (int) this->session_key[i] << " ";
    }
    std::cout << std::dec << "\n";

    return 0;
}

unsigned char* KeyExchange::get_public_key()
{
    return reinterpret_cast<unsigned char*>(&this->p_public);
}
