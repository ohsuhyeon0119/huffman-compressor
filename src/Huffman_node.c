#include <stdio.h>
#include <stdlib.h>
#include "Huffman_node.h"



Huffman_node* Huffman_node_allocate() {
    Huffman_node* node = (Huffman_node*) malloc(sizeof(Huffman_node));
    if (node == NULL) {
        perror("Error: Memory allocation failed for Huffman_node.");
        exit(EXIT_FAILURE);
    }
    node->l = NULL; 
    node->r = NULL; 
    return node;
}

void Huffman_node_initialize(Huffman_node* target, uint8_t ch, uint64_t cnt, Huffman_node* l, Huffman_node* r) {
    if (target == NULL) {
        fprintf(stderr, "Error: Cannot initialize a NULL node.\n");
        return;
    }
    target->ch = ch;
    target->cnt = cnt;
    target->l = l;
    target->r = r;
}

Huffman_node* Huffman_node_create(uint8_t ch, uint64_t cnt, Huffman_node* l, Huffman_node* r) {
    Huffman_node* node = (Huffman_node*) malloc(sizeof(Huffman_node));
    if (node == NULL) {
        perror("Error: Memory allocation failed for Huffman_node.");
        exit(EXIT_FAILURE);
    }
    node->ch = ch;
    node->cnt = cnt;
    node->l = l;
    node->r = r;
    return node;
}

void Huffman_node_deallcoate(Huffman_node* self) {
    if (self == NULL) {
        fprintf(stderr, "Error: Cannot deallocate a NULL node.\n");
        return;
    }
    free(self);
}


int Huffman_node_compare(const Huffman_node* a, const Huffman_node* b) {
    if (a == NULL || b == NULL) {
        fprintf(stderr, "Error: Cannot compare NULL nodes.\n");
        return 0; 
    }

    if (a->cnt < b->cnt) return -1; 
    if (a->cnt > b->cnt) return 1;  
    return 0; 
}

int Huffman_node_is_leaf(const Huffman_node* node) {
    if (node == NULL) {
        fprintf(stderr, "Error: Cannot check leaf status of a NULL node.\n");
        return 0; 
    }

    return (node->l == NULL && node->r == NULL); 
}

void Huffman_node_print(const Huffman_node* node) {
    if (node == NULL) {
        printf("Node: NULL\n");
        return;
    }

    
    char is_leaf = Huffman_node_is_leaf(node) ? 'Y' : 'N';

    printf("Node Info:\n");
    printf("  Char: '%c'\n", (node->ch != '\0') ? node->ch : '-'); 
    printf("  Count: %lu\n", node->cnt); 
    printf("  Is Leaf: %c\n", is_leaf); 
    printf("  Left Child: %s\n", (node->l != NULL) ? "Exists" : "NULL");
    printf("  Right Child: %s\n", (node->r != NULL) ? "Exists" : "NULL");
}