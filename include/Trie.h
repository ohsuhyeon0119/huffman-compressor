#ifndef TRIE_H
#define TRIE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct TrieNode TrieNode;


struct TrieNode {
    struct TrieNode* left;  
    struct TrieNode* right; 
    uint8_t is_leaf;        
    uint8_t character;      
} ;

TrieNode* TrieNode_create();

TrieNode* Trie_build_from_metadata(const uint8_t* metadata, size_t metadata_size);


void Trie_destroy(TrieNode* root);

void decode(TrieNode* root, uint8_t* data, size_t total_bits, FILE* outputFile);

#endif
