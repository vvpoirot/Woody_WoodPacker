#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <zlib.h>
#include <stdint.h>

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


Node*   create_node(unsigned char character, unsigned frequency);
void    init_priority_queue(PriorityQueue* pq);
void    enqueue(PriorityQueue* pq, Node* node);
Node*   dequeue(PriorityQueue* pq);
Node*   build_huffman_tree(unsigned int* frequencies);
void    generate_huffman_codes(Node* root, char* codes[], char* code, int depth);
void    serialize_tree(Node* node, int fd);
void    compress_data(int fd_in, int fd_out, char* codes[]);
Node*   deserialize_tree(int fd);
void    decompress_data(int fd_in, int fd_out);
void    huffman(int fd_in, int fd_out, int mode);


void    encrypt(uint32_t *v, const uint32_t *k);
void    decrypt(uint32_t *v, const uint32_t *k);
void    generate_key(uint32_t *key, size_t keySize);
void    TEA(int inputFD, int outputFD, int mode);
