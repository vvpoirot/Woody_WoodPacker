#include "lib.h"

// TEA decryption function for memory
void decrypt(uint32_t *v, const uint32_t *k) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0xC6EF3720;
    uint32_t delta = 0x9E3779B9;
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];

    for (int i = 0; i < 32; i++) {
        v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
        v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        sum -= delta;
    }
    v[0] = v0;
    v[1] = v1;
}

void TEA_decrypt(char *input, char *output, size_t size) {
    uint32_t key[4];

    memcpy(key, input, sizeof(key));
    input += sizeof(key);

    for (size_t i = 0; i < size; i += 8) {
        uint32_t block[2];
        memcpy(block, input + i, sizeof(block));

        decrypt_memory(block, key);
        memcpy(output + i, block, sizeof(block));
    }
}

