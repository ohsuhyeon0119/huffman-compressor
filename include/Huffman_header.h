#ifndef HUFFMAN_HEADER_H
#define HUFFMAN_HEADER_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>


typedef struct {
    uint32_t magic_number;             
    uint32_t header_size;              
    uint64_t file_size;                
    uint32_t codeword_map_metadata_size; 
    uint8_t* codeword_map_metadata;    
} Huffman_header;


Huffman_header* Huffman_header_create(uint32_t magic_number, uint64_t file_size, uint32_t metadata_size, uint8_t* metadata);

uint8_t* Huffman_header_serialize(const Huffman_header* header, size_t* serialized_size);

void Huffman_header_destroy(Huffman_header* header);

Huffman_header* Huffman_header_deserialize(FILE* file);



#endif 
