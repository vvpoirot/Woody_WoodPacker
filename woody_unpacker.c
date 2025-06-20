#include "lib.h"

void ft_error(char* msg) {
    write(2, "[ERROR] ", 8);
    write(2, msg, strlen(msg));
    exit(EXIT_FAILURE);
}

int main(int argc, char**argv) {
    if (argc != 2)
        ft_error("Usage: ./woody_unpacker <encrypted>\n");

    int inputFD = open(argv[1], O_RDONLY);
    int decryptedFD = open("decrypted", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    TEA(inputFD, decryptedFD, 1);
    int decryptedInputFD = open("decrypted", O_RDONLY);
    int decompressedFD = open("decompressed", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    huffman(decryptedInputFD, decompressedFD, 1);

    close(inputFD);
    close(decryptedFD);
    close(decryptedInputFD);
    close(decompressedFD);
    return EXIT_SUCCESS;
}