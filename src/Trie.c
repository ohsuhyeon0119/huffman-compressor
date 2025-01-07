#include "Trie.h"


TrieNode* TrieNode_create() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    if (!node) {
        perror("Failed to allocate TrieNode");
        exit(EXIT_FAILURE);
    }
    node->left = NULL;
    node->right = NULL;
    node->is_leaf = 0; 
    node->character = 0; 
    return node;
}


TrieNode* Trie_build_from_metadata(const uint8_t* metadata, size_t metadata_size) {
    TrieNode* root = TrieNode_create(); 
    size_t offset = 0;

    while (offset < metadata_size) {
        uint8_t target_character = metadata[offset++]; 
        uint8_t codeword_length = metadata[offset++];  

        
        TrieNode* current = root;
        for (int i = 0; i < codeword_length; i++) {
            
            uint8_t bit = (metadata[offset + (i / 8)] >> (7 - (i % 8))) & 1;

            
            if (bit == 0) {
                if (!current->left) {
                    current->left = TrieNode_create();
                }
                current = current->left;
            } else {
                if (!current->right) {
                    current->right = TrieNode_create();
                }
                current = current->right;
            }
        }

        
        current->is_leaf = 1;
        current->character = target_character;

        
        offset += (codeword_length + 7) / 8;
    }

    return root;
}


void Trie_destroy(TrieNode* root) {
    if (!root) return;

    Trie_destroy(root->left);
    Trie_destroy(root->right);
    free(root);
}

void decode(TrieNode* root, uint8_t* data, size_t total_bits, FILE* outputFile) {
    TrieNode* current = root; 
    size_t bit_offset = 0;    

    
    while (bit_offset < total_bits) {
        
        size_t byte_index = bit_offset / 8;    
        size_t bit_index = bit_offset % 8;    
        uint8_t bit = (data[byte_index] >> (7 - bit_index)) & 1; 

        
        if (bit == 0) {
            current = current->left;
        } else {
            current = current->right;
        }

        
        if (current->is_leaf) {
            
            fwrite(&current->character, 1, 1, outputFile);
            
            current = root;
        }

        
        bit_offset++;
    }
}