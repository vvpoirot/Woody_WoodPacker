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

typedef struct PriorityQueue {
    int size;
    Node* nodes[NUM_SYMBOLS];
} PriorityQueue;

void ft_error(char* msg);
void huffman(int fd_in, int fd_out, int mode);
off_t getFileSize(int fd);
void    ft_putnbr(int fd, int nb);
