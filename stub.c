#include "lib.h"

void ft_error(char* msg) {
    write(2, "[ERROR] ", 8);
    write(2, msg, strlen(msg));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    (void)argc;
    int fd = open(argv[0], O_RDONLY);
    if (fd == -1)
        ft_error("Failed to open file.\n");

    int tempFD = open("temp", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (tempFD == -1) {
        close(fd);
        ft_error("Failed to create temporary file.\n");
    }

    TEA(fd, tempFD, 1);
    lseek(tempFD, 0, SEEK_SET);

    int decompressedFD = open("decompressed", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (decompressedFD == -1) {
        close(fd);
        close(tempFD);
        ft_error("Failed to create decompressed file.\n");
    }

    lseek(fd, 0, SEEK_SET);
    huffman(fd, decompressedFD, 1);

    close(fd);
    close(tempFD);
    close(decompressedFD);

    execl("./decompressed", "./decompressed", NULL);

    unlink("temp");
    unlink("decompressed");

    return EXIT_SUCCESS;

}