#include "srcs/lib.h"

extern char _binary_payload_bin_start;
extern char _binary_payload_bin_end;


void ft_error(char* msg) {
    write(2, "[ERROR] ", 8);
    write(2, msg, strlen(msg));
    exit(EXIT_FAILURE);
}

off_t getFileSize(int fd) {
    off_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    return size;
}

int main(int argc, char**argv) {
    if (argc != 2)
        ft_error("Usage: ./woody_unpacker <encrypted>\n");

    char* payloadData = &_binary_payload_bin_start;
    int payloadSize = &_binary_payload_bin_end - &_binary_payload_bin_start;
    if (payloadSize <= 0)
        ft_error("Payload size is zero or negative.\n");

    char* decryptedData = malloc(payloadSize); // jpense faut faire un mmap pour changer les flags et pouvoir l'executer directement dans la memoire allouer et pas passer par un fichier extern
    if (!decryptedData)
        ft_error("Memory allocation failed.\n");

    // remplacer les fichiers par le payload directement en memoire.
    int inputFD = open(argv[1], O_RDONLY);
    int decryptedFD = open("decrypted", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    char* in_buf = malloc(getFileSize(inputFD));
    read(inputFD, in_buf, getFileSize(inputFD));
    char* output_buf = malloc(/* taille du fichier une fois decryter  */);
    TEA_decrypt(in_buf, output_buf, (size_t)getFileSize(inputFD)); // dechiffrer le payload
    // TEA_decrypt(inputFD, decryptedFD, (size_t)getFileSize(inputFD)); // dechiffrer le payload
    close(inputFD);
    close(decryptedFD);
    
    // pareille pour la decompressions
    int decryptedInputFD = open("decrypted", O_RDONLY);
    int decompressedFD = open("decompressed", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    huffman(decryptedInputFD, decompressedFD);// Decompress the decrypted file
    close(decryptedInputFD);
    close(decompressedFD);

    return EXIT_SUCCESS;
}
