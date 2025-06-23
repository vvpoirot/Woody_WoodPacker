#include "srcs/lib.h"

extern char _binary_payload_bin_start;
extern char _binary_payload_bin_end;


void ft_error(char* msg) {
    write(2, "[ERROR] ", 8);
    write(2, msg, strlen(msg));
    exit(EXIT_FAILURE);
}

int main(int argc, char**argv) {
    if (argc != 2)
        ft_error("Usage: ./woody_unpacker <encrypted>\n");

    char* payloadData = &_binary_payload_bin_start;
    int payloadSize = &_binary_payload_bin_end - &_binary_payload_bin_start;
    if (payloadSize <= 0)
        ft_error("Payload size is zero or negative.\n");

    char* decryptedData = malloc(payloadSize);
    if (!decryptedData)
        ft_error("Memory allocation failed.\n");

    TEA(payloadData, decryptedData, payloadSize, 1);





    // remplacer les fichiers par le payload directement en memoire.
    int inputFD = open(argv[1], O_RDONLY);
    int decryptedFD = open("decrypted", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    TEA(inputFD, decryptedFD, 1); // dechiffrer le payload
    close(inputFD);
    close(decryptedFD);
    
    // pareille pour la decompressions
    int decryptedInputFD = open("decrypted", O_RDONLY);
    int decompressedFD = open("decompressed", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    huffman(decryptedInputFD, decompressedFD, 1);// Decompress the decrypted file
    close(decryptedInputFD);
    close(decompressedFD);

    return EXIT_SUCCESS;
}