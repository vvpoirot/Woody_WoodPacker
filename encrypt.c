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

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <encrypt|decrypt> <input file> <output file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *mode = argv[1];
    const char *inputFilename = argv[2];
    const char *outputFilename = argv[3];

    int inputFd = open(inputFilename, O_RDONLY);
    if (inputFd == -1) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    int outputFd = open(outputFilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (outputFd == -1) {
        perror("Error opening output file");
        close(inputFd);
        return EXIT_FAILURE;
    }

    uint32_t block[2];
    uint32_t key[] = {0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210}; // 128-bit key

    ssize_t bytesRead;
    while ((bytesRead = read(inputFd, block, sizeof(block))) > 0) {
        if (bytesRead == sizeof(block)) {
            if (strcmp(mode, "encrypt") == 0) {
                encrypt(block, key);
            } else if (strcmp(mode, "decrypt") == 0) {
                decrypt(block, key);
            } else {
                fprintf(stderr, "Invalid mode: %s\n", mode);
                close(inputFd);
                close(outputFd);
                return EXIT_FAILURE;
            }
            write(outputFd, block, sizeof(block));
        } else {
            // Handle the last block if it's not a complete 64-bit block
            uint8_t lastBlock[sizeof(block)];
            memcpy(lastBlock, block, bytesRead);
            for (size_t i = bytesRead; i < sizeof(block); i++) {
                lastBlock[i] = 0; // Pad with zeros
            }
            memcpy(block, lastBlock, sizeof(block));

            if (strcmp(mode, "encrypt") == 0) {
                encrypt(block, key);
            } else if (strcmp(mode, "decrypt") == 0) {
                decrypt(block, key);
            } else {
                fprintf(stderr, "Invalid mode: %s\n", mode);
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
