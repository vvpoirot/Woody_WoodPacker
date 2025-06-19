#include "lib.h"

// Fonction pour créer un nouveau nœud
Node* create_node(unsigned char character, unsigned frequency) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->character = character;
    node->frequency = frequency;
    node->left = node->right = NULL;
    return node;
}

// Fonction pour initialiser une file de priorité
void init_priority_queue(PriorityQueue* pq) {
    pq->size = 0;
}

// Fonction pour insérer un nœud dans la file de priorité
void enqueue(PriorityQueue* pq, Node* node) {
    int i = pq->size++;
    while (i && node->frequency < pq->nodes[(i - 1) / 2]->frequency) {
        pq->nodes[i] = pq->nodes[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    pq->nodes[i] = node;
}

// Fonction pour extraire le nœud avec la fréquence la plus basse
Node* dequeue(PriorityQueue* pq) {
    Node* node = pq->nodes[0];
    Node* last_node = pq->nodes[--pq->size];
    int i = 0, child;
    while ((child = 2 * i + 1) < pq->size) {
        if (child + 1 < pq->size && pq->nodes[child + 1]->frequency < pq->nodes[child]->frequency) {
            child++;
        }
        if (last_node->frequency > pq->nodes[child]->frequency) {
            pq->nodes[i] = pq->nodes[child];
        } else {
            break;
        }
        i = child;
    }
    pq->nodes[i] = last_node;
    return node;
}

// Fonction pour construire l'arbre de Huffman
Node* build_huffman_tree(unsigned int* frequencies) {
    PriorityQueue pq;
    init_priority_queue(&pq);

    for (int i = 0; i < NUM_SYMBOLS; i++) {
        if (frequencies[i] > 0) {
            enqueue(&pq, create_node((unsigned char)i, frequencies[i]));
        }
    }

    while (pq.size > 1) {
        Node* left = dequeue(&pq);
        Node* right = dequeue(&pq);
        Node* parent = create_node('\0', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        enqueue(&pq, parent);
    }

    return dequeue(&pq);
}

// Fonction pour générer les codes de Huffman
void generate_huffman_codes(Node* root, char* codes[], char* code, int depth) {
    if (root == NULL) {
        return;
    }

    if (root->left == NULL && root->right == NULL) {
        code[depth] = '\0';
        codes[root->character] = strdup(code);
        return;
    }

    if (root->left != NULL) {
        code[depth] = '0';
        generate_huffman_codes(root->left, codes, code, depth + 1);
    }

    if (root->right != NULL) {
        code[depth] = '1';
        generate_huffman_codes(root->right, codes, code, depth + 1);
    }
}

// Fonction pour sérialiser l'arbre de Huffman
void serialize_tree(Node* node, int fd) {
    if (node == NULL) {
        return;
    }

    if (node->left == NULL && node->right == NULL) {
        char leafMarker = '1';
        write(fd, &leafMarker, 1);
        write(fd, &node->character, 1);
    } else {
        char internalMarker = '0';
        write(fd, &internalMarker, 1);
        serialize_tree(node->left, fd);
        serialize_tree(node->right, fd);
    }
}

// Fonction pour compresser les données
void compress_data(int fd_in, int fd_out, char* codes[]) {
    char buffer[1024];
    ssize_t bytes_read;
    char bit_buffer = 0;
    int bit_count = 0;

    lseek(fd_in, 0, SEEK_SET);

    while ((bytes_read = read(fd_in, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            char* code = codes[(unsigned char)buffer[i]];
            if (code == NULL) {
                continue;
            }

            for (int j = 0; code[j] != '\0'; j++) {
                bit_buffer = (bit_buffer << 1) | (code[j] - '0');
                bit_count++;

                if (bit_count == 8) {
                    write(fd_out, &bit_buffer, 1);
                    bit_buffer = 0;
                    bit_count = 0;
                }
            }
        }
    }

    if (bit_count > 0) {
        bit_buffer <<= (8 - bit_count);
        write(fd_out, &bit_buffer, 1);
    }
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

// Fonction pour décompresser les données
void decompress_data(int fd_in, int fd_out) {
    Node* root = deserialize_tree(fd_in);
    if (root == NULL) {
        fprintf(stderr, "Failed to deserialize Huffman tree\n");
        return;
    }

    Node* current = root;
    unsigned char byte;
    int bit_index;

    while (read(fd_in, &byte, 1) == 1) {
        for (bit_index = 7; bit_index >= 0; --bit_index) {
            int bit = (byte >> bit_index) & 1;
            current = bit ? current->right : current->left;

            if (current == NULL) {
                fprintf(stderr, "Invalid bit sequence or corrupted Huffman tree\n");
                return;
            }

            if (current->left == NULL && current->right == NULL) {
                write(fd_out, &current->character, 1);
                current = root;
            }
        }
    }
}

// Fonction principale pour compresser ou décompresser
void huffman(int fd_in, int fd_out, int mode) {

    if (mode == 0) {
        // Mode compression
        unsigned int frequencies[NUM_SYMBOLS] = {0};
        char buffer[1024];
        ssize_t bytes_read;

        while ((bytes_read = read(fd_in, buffer, sizeof(buffer))) > 0) {
            for (ssize_t i = 0; i < bytes_read; i++) {
                frequencies[(unsigned char)buffer[i]]++;
            }
        }

        Node* root = build_huffman_tree(frequencies);

        char* codes[NUM_SYMBOLS] = {NULL};
        char code[NUM_SYMBOLS];
        generate_huffman_codes(root, codes, code, 0);

        serialize_tree(root, fd_out);
        compress_data(fd_in, fd_out, codes);

        for (int i = 0; i < NUM_SYMBOLS; i++) {
            if (codes[i] != NULL) {
                free(codes[i]);
            }
        }
    } else {
        // Mode décompression
        decompress_data(fd_in, fd_out);
    }
}