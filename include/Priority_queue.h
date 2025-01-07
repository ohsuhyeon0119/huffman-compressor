#ifndef PQ_H
#define PQ_H
#include "Huffman_node.h"
#include <stdlib.h>


typedef struct PriorityQueue PriorityQueue;

struct PriorityQueue{
    Huffman_node** arr;                   
    int size;                      
    int capacity;                  
} ;

PriorityQueue* Pq_create(int capacity);

void Pq_destroy(PriorityQueue* pq);

void Pq_pushNode(PriorityQueue* pq, Huffman_node* element);

Huffman_node* Pq_top(PriorityQueue* pq) ;

Huffman_node* Pq_pop(PriorityQueue* pq) ;

#endif