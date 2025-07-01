/**
 * TODO_LIST:
 * > file -> buffer -> outfile et exec le outfile X
 * > func: take buffer -> set in memory -> exec it
 * > func: decompressed buffer and exec it
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <memory.h>
#include <stdint.h>
#define _GNU_SOURCE

// Wrapper memfd_create si non défini dans libc
#ifndef MFD_CLOEXEC
#define MFD_CLOEXEC 0x0001U
#endif

#ifndef SYS_memfd_create
#if defined(__x86_64__)
#define SYS_memfd_create 319
#elif defined(__i386__)
#define SYS_memfd_create 356
#else
#error "Architecture non supportée"
#endif
#endif

int memfd_create(const char *name, unsigned int flags) {
    return syscall(SYS_memfd_create, name, flags);
}

// O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH

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

// Fonction pour créer un nouveau nœud
Node* create_node(unsigned char character, unsigned frequency) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->character = character;
    node->frequency = frequency;
    node->left = node->right = NULL;
    return node;
}

// Fonction pour désérialiser l'arbre de Huffman
Node* deserialize_tree(int fd) {
    char node_type;
    if (read(fd, &node_type, 1) != 1) {
        return NULL;
    }

    if (node_type == '1') {
        unsigned char character;
        if (read(fd, &character, 1) != 1) {
            return NULL;
        }
        return create_node(character, 0);
    } else if (node_type == '0') {
        Node* node = create_node('\0', 0);
        node->left = deserialize_tree(fd);
        node->right = deserialize_tree(fd);
        return node;
    }

    return NULL;
}

void memexec(char *membuf, size_t sizebuf, char *const argv[], char *const envp[]) {
    int fd = memfd_create("out", MFD_CLOEXEC);
    if (fd == -1)
        ft_error("memfd_create\n");
    ssize_t written = write(fd, membuf, sizebuf);
    if (written != (ssize_t)sizebuf) {
        close(fd);
        ft_error("write\n");
    }
    char path[64];
    snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);
    // Lancer execve sur ce chemin
    if (execve(path, argv, envp) == -1) {
        perror("execve");
        close(fd);
        ft_error("End of program\n");
    }
}

// Fonction pour décompresser les données
void decompress_data(int fd_in) {
    Node* root = deserialize_tree(fd_in);
    if (root == NULL) {
        ft_error("Failed to deserialize Huffman tree\n");
        return;
    }

    size_t decompressed_size = 1048576; // Valeur temporaire fixe
    size_t page_size = sysconf(_SC_PAGESIZE);

    // Assurez-vous que la taille est un multiple de la taille de page
    decompressed_size = (decompressed_size + page_size - 1) & ~(page_size - 1);
    // size_t decompressed_size = get_decompressed_size(); // You need to implement this
    char* membuf = malloc(decompressed_size);
    if (membuf == NULL) {
        ft_error("Failed to allocate memory for decompressed data\n");
        return;
    }

    Node* current = root;
    unsigned char byte;
    int bit_index;
    size_t buffer_index = 0;
    
    int fd_out = open("bin", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    
    while (read(fd_in, &byte, 1) == 1) {
        for (bit_index = 7; bit_index >= 0; --bit_index) {
            int bit = (byte >> bit_index) & 1;
            current = bit ? current->right : current->left;

            if (current == NULL) {
                ft_error("Invalid bit sequence or corrupted Huffman tree\n");
                free(membuf);
                return;
            }

            if (current->left == NULL && current->right == NULL) {
                if (buffer_index < decompressed_size) {
                    write(fd_out, &current->character, 1);
                    membuf[buffer_index++] = (char)current->character;
                } else {
                    ft_error("Decompressed data exceeds expected size\n");
                    free(membuf);
                    return;
                }
                current = root;
            }
        }
    }

    close(fd_out);
    //memexec(membuf, decompressed_size);
    free(membuf);
}

char *read_file_to_buffer(int fd, size_t *size_out) {
    if (fd < 0) return NULL;

    // Obtenir la taille du fichier avec lseek
    off_t size = lseek(fd, 0, SEEK_END);
    if (size == -1) return NULL;

    // Revenir au début du fichier
    if (lseek(fd, 0, SEEK_SET) == -1) return NULL;

    // Allouer le buffer
    char *buffer = malloc(size);
    if (!buffer) return NULL;

    // Lire le contenu
    ssize_t n = read(fd, buffer, size);
    if (n != size) {
        free(buffer);
        return NULL;
    }

    if (size_out) *size_out = size;
    return buffer;
}

int main(int argc, char** argv, char** envp) {
    if (argc != 2)
        ft_error("Wrong number of arg\n");
    int fd_in = open(argv[1], O_RDONLY);
    //decompress_data(fd_in);
    size_t size;
    char* buffer = read_file_to_buffer(fd_in, &size);
    char *av[] = { "out", NULL };
    memexec(buffer, size, av, envp);
    // int fd_out = open("out", O_CREAT | O_WRONLY | O_TRUNC, 0700);
    // write(fd_out, buffer, size);
    close(fd_in);
    free(buffer);
    return 0;
}