#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/syscall.h>

// Wrapper memfd_create si non défini dans libc
#ifndef MFD_CLOEXEC
#define MFD_CLOEXEC 0x0001U
#endif

int memfd_create(const char *name, unsigned int flags) {
    return syscall(SYS_memfd_create, name, flags);
}

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
    if (syscall(SYS_execve, path, argv, envp) == -1) {
        perror("execve");
        close(fd);
        ft_error("End of program\n");
    }
}

// Fonction pour décompresser les données
void decompress_data(int fd_in, char** envp) {
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

    char* av[] = { "out", NULL };
    memexec(membuf, decompressed_size, av, envp);
    free(membuf);
}

int main(int argc, char** argv, char** envp) {
    if (argc != 2)
        ft_error("Wrong number of arg\n");
    int fd_in = open(argv[1], O_RDONLY);
    decompress_data(fd_in, envp);
    return 0;
}