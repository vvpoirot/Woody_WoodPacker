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


// get key from argv[4]
void get_key(uint32_t *key, const char *key_str) {
    size_t len = strlen(key_str);
    if (len != 32) {
        printf("Error: Key must be 32 hexadecimal characters long.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 4; i++) {
        char hex[9];
        strncpy(hex, key_str + i * 8, 8);
        hex[8] = '\0';
        key[i] = (uint32_t)strtoul(hex, NULL, 16);
    }
}

int main(int argc, char *argv[]) {
    const char *mode = argv[1];
    const char *inputFilename = argv[2];
    const char *outputFilename = argv[3];

    int inputFd = open(inputFilename, O_RDONLY);
    if (inputFd == -1) {
        printf("Error opening input file\n");
        return EXIT_FAILURE;
    }

    int outputFd = open(outputFilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (outputFd == -1) {
        printf("Error opening output file\n");
        close(inputFd);
        return EXIT_FAILURE;
    }

    uint32_t block[2];
    uint32_t key[4];

    if (strcmp(mode, "encrypt") == 0 && argc == 4) {
        generate_key(key, sizeof(key));
        printf("Generated key: %08X%08X%08X%08X\n", key[0], key[1], key[2], key[3]);
    }

    ssize_t bytesRead;
    while ((bytesRead = read(inputFd, block, sizeof(block))) > 0) {
        if (bytesRead == sizeof(block)) {
            if (strcmp(mode, "encrypt") == 0 && argc == 4) {
                encrypt(block, key);
            } else if (strcmp(mode, "decrypt") == 0 && argc == 5) {
                get_key(key, argv[4]);
                decrypt(block, key);
            } else {
                printf("Usage: %s <encrypt|decrypt> <input file> <output file> <decrypt key>\n", argv[0]);
                close(inputFd);
                close(outputFd);
                return EXIT_FAILURE;
            }
            write(outputFd, block, sizeof(block));
        } else {
            // Handle the last block if it's not a complete 64-bit block
            uint8_t lastBlock[sizeof(block)];
            memcpy(lastBlock, block, bytesRead);
            for (size_t i = bytesRead; i < sizeof(block); i++)
                lastBlock[i] = 0; // Pad with zeros
            memcpy(block, lastBlock, sizeof(block));

            if (strcmp(mode, "encrypt") == 0) {
                encrypt(block, key);
            } else if (strcmp(mode, "decrypt") == 0) {
                decrypt(block, key);
            } else {
                printf("Invalid mode: %s\n", mode);
                close(inputFd);
                close(outputFd);
                return EXIT_FAILURE;
            }
            write(outputFd, block, sizeof(block));
        }
    }

    close(inputFd);
    close(outputFd);

    printf("File processed successfully.\n");
    return EXIT_SUCCESS;
}
