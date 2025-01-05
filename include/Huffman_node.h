#ifndef HUFFMAN_NODE_H
#define HUFFMAN_NODE_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 

typedef struct Huffman_node {
    uint8_t ch;                
    uint64_t cnt;              
    struct Huffman_node* l;    
    struct Huffman_node* r;    
} Huffman_node;

Huffman_node* Huffman_node_allocate();

void Huffman_node_initialize(Huffman_node* target, uint8_t ch, uint64_t cnt, Huffman_node* l, Huffman_node* r);

Huffman_node* Huffman_node_create(uint8_t ch, uint64_t cnt, Huffman_node* l, Huffman_node* r);

void Huffman_node_deallcoate(Huffman_node* self);

int Huffman_node_compare(const Huffman_node* a, const Huffman_node* b);

int Huffman_node_is_leaf(const Huffman_node* node);

void Huffman_node_print(const Huffman_node* node);





#endif 
