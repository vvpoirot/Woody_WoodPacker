#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// TEA encryption function
void encrypt(uint32_t *v, const uint32_t *k) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0;
    uint32_t delta = 0x9E3779B9;
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];

    for (int i = 0; i < 32; i++) {
        sum += delta;
        v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
    }
    v[0] = v0;
    v[1] = v1;
}

// TEA decryption function
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

void generate_key(uint32_t *key, size_t keySize) {
    int randomData = open("/dev/urandom", O_RDONLY);
    if (randomData < 0) {
        printf("Error opening /dev/urandom");
        exit(EXIT_FAILURE);
    }
    ssize_t result = read(randomData, key, keySize);
    if (result < 0) {
        printf("Error reading from /dev/urandom");
        close(randomData);
        exit(EXIT_FAILURE);
    }
    close(randomData);
}

void TEA(char* input, char* output, int size ,int mode) {
    uint32_t block[2];
    uint32_t key[4];

    if (mode == 0) {
        generate_key(key, sizeof(key));
        printf("Generated key: %08X%08X%08X%08X\n", key[0], key[1], key[2], key[3]);
    } else {
        if (read(inputFD, key, sizeof(key)) != sizeof(key)) {
            printf("Error reading key from input file.\n");
            close(inputFD);
            close(outputFD);
            exit(EXIT_FAILURE);
        }
    }

    ssize_t bytesRead;
    while ((bytesRead = read(inputFD, block, sizeof(block))) > 0) {
        if (bytesRead == sizeof(block)) {
            if (mode == 0) {
                encrypt(block, key);
            } else {
                decrypt(block, key);
            }
            write(outputFD, block, sizeof(block));
        } else {
            // Handle the last block if it's not a complete 64-bit block
            uint8_t lastBlock[sizeof(block)];
            memcpy(lastBlock, block, bytesRead);
            for (size_t i = bytesRead; i < sizeof(block); i++)
                lastBlock[i] = 0; // Pad with zeros
            memcpy(block, lastBlock, sizeof(block));

            if (mode == 0) {
                encrypt(block, key);
            } else {
                decrypt(block, key);
                write(outputFD, block, sizeof(block));
            }
        }
    }
}
