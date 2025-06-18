#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <zlib.h>

#define NUM_SYMBOLS 256
typedef struct Node {
    unsigned char character;
    unsigned int frequency;
    struct Node *left, *right;
} Node;

void ft_error(char* msg)
{
    write(2, "[ERROR] ", strlen("[ERROR] "));
    write(2, msg, strlen(msg));
    exit(EXIT_FAILURE);
}

void validArg(char* arg)
{
    int fd = open(arg, O_RDONLY);
    if (fd < 0 || read(fd, 0, 0) == -1)
        ft_error("File can't be open\n");

    unsigned char header[6];
    header[6] = 0;
    if (read(fd, header, 5) == -1)
        ft_error("Can't take content of file\n");
    close(fd);

    if (header[0] != 0x7F || header[1] != 'E' || header[2] != 'L' || header[3] != 'F' || header[4] != 2)
        ft_error("File architecture not suported. x86_64 only.\n");
}

off_t getFileSize(int fd) {
    off_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    return (size);
}

void build(Node* root, unsigned int* frequencies) {
    unsigned int high_frec = 0;
    int index;
    for (ssize_t i = 0; i < NUM_SYMBOLS; i++) {
        if (frequencies[i] >= high_frec)
            index = i;
    }
}

void ft_compress(int inputFD, int outputFD) {
    off_t buff_len = getFileSize(inputFD);
    char* buff = (char*)malloc((long long)buff_len);
    if (!buff) {
        close(inputFD);
        close(outputFD);
        ft_error("Malloc crash");
    }
    ssize_t bytes = read(inputFD, buff, sizeof(buff_len));
    if (bytes == -1) {
        close(inputFD);
        close(outputFD);
        free(buff);
        ft_error("Read crash\n");
    }

    Node* root;

    unsigned int frequencies[NUM_SYMBOLS] = {0};
    for (ssize_t i = 0; i < bytes; i++)
        frequencies[(unsigned char)buff[i]]++;

    build(&root, &frequencies);
    //generate codes
    //write in output FD

    free(buff);
    printf("File compressed!\n");
}

void ft_uncompress(int inputFD, int outputFD) {
    off_t input_len = getFileSize(inputFD);
    char* input_buffer = malloc((long long)input_len);
    if (input_buffer == NULL) {
        close(inputFD);
        close(outputFD);
        ft_error("Malloc crash #1\n");
    }
    if (read(inputFD, input_buffer, (long long)input_len) == -1) {
        close(outputFD);
        close(inputFD);
        free(input_buffer);
        ft_error("Read crash\n");
    }

    uLongf output_len = input_len * 100;
    Bytef* output_buffer = (Bytef*)malloc(output_len);
    if (!output_buffer) {
        close(outputFD);
        close(inputFD);
        free(input_buffer);
        ft_error("Malloc crash #2\n");
    }

    int res = uncompress(output_buffer, &output_len, (const Bytef*)input_buffer, input_len);
    if (res != Z_OK) {
        close(outputFD);
        close(inputFD);
        free(input_buffer);
        free(output_buffer);
        ft_error("Uncompress crash\n");
    }
    write(outputFD, output_buffer, output_len);
    free(input_buffer);
    free(output_buffer);
    printf("Uncompress complete !\n");
}

int main(int argc, char** argv) {
    if (argc != 2)
        ft_error("Wrong number of arg\n");
    validArg(argv[1]);

    int inputFD = open(argv[1], O_RDONLY);
    int outputFD = open("woody", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    ft_compress(inputFD, outputFD);
    close(inputFD);
    close(outputFD);

    // int woodyFD = open("woody", O_RDONLY);
    // int destFD = open("pecker", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    // ft_uncompress(woodyFD, destFD);
    // close(woodyFD);
    // close(destFD);
    return (EXIT_SUCCESS);
}