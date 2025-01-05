#ifndef HUFFMAN_TREE_UTIL_H
#define HUFFMAN_TREE_UTIL_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Huffman_node.h"
#include "PriorityQueue.h"
#include "Byte_table.h"

Huffman_node* Huffman_tree_generate(PriorityQueue* pq) ;

void Huffman_tree_fill_codewords(Huffman_node* node, uint8_t* code, int depth, ByteTable* bt);

#endif 
