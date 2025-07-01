#include "srcs/lib.h"

void ft_error(char* msg) {
    write(2, "[ERROR] ", 8);
    write(2, msg, strlen(msg));
    exit(EXIT_FAILURE);
}

void validArg(char* arg) {
    int fd = open(arg, O_RDONLY);
    if (fd < 0 || read(fd, 0, 0) == -1)
        ft_error("Failed to open binary file.\n");

    unsigned char header[6];
    header[6] = 0;
    if (read(fd, header, 5) == -1)
        ft_error("Failed to read content of file.\n");
    close(fd);

    if (header[0] != 0x7F || header[1] != 'E' || header[2] != 'L' || header[3] != 'F' || header[4] != 2)
        ft_error("File architecture not supported. x86_64 only.\n");
}

off_t getFileSize(int fd) {
    off_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    return size;
}

int main(int argc, char** argv) {
    if (argc != 2)
        ft_error("Usage: ./woody_woodpacker <binary>\n");
    validArg(argv[1]);

    int inputFD = open(argv[1], O_RDONLY);
    int compressedFD = open("compressed", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    huffman(inputFD, compressedFD);

    // int compressedInputFD = open("compressed", O_RDONLY);
    // int encryptedFD = open("encrypted", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    // TEA(compressedInputFD, encryptedFD, 0);

    close(inputFD);
    close(compressedFD);
    // close(compressedInputFD);
    // close(encryptedFD);
    return EXIT_SUCCESS;
}