#include "Huffman_header.h"



Huffman_header* Huffman_header_create(uint32_t magic_number, uint64_t file_size, uint32_t metadata_size, uint8_t* metadata) {
    Huffman_header* header = (Huffman_header*)malloc(sizeof(Huffman_header));
    if (!header) {
        perror("Failed to allocate memory for Huffman_header");
        exit(EXIT_FAILURE);
    }

    header->magic_number = magic_number;
    header->file_size = file_size;
    header->codeword_map_metadata_size = metadata_size;

    
    header->codeword_map_metadata = (uint8_t*)malloc(metadata_size);
    if (!header->codeword_map_metadata) {
        perror("Failed to allocate memory for codeword_map_metadata");
        free(header);
        exit(EXIT_FAILURE);
    }
    memcpy(header->codeword_map_metadata, metadata, metadata_size);

    
    header->header_size = 4 + 4 + 8 + 4 + metadata_size;

    return header;
}




uint8_t* Huffman_header_serialize(const Huffman_header* header, size_t* serialized_size) {
    if (!header || !serialized_size) {
        fprintf(stderr, "Invalid arguments to Huffman_header_serialize\n");
        return NULL;
    }

    
    *serialized_size = header->header_size;

    
    uint8_t* buffer = (uint8_t*)malloc(*serialized_size);
    if (!buffer) {
        perror("Failed to allocate memory for serialized Huffman_header");
        exit(EXIT_FAILURE);
    }

    
    size_t offset = 0;

    
    memcpy(buffer + offset, &header->magic_number, sizeof(header->magic_number));
    offset += sizeof(header->magic_number);

    
    memcpy(buffer + offset, &header->header_size, sizeof(header->header_size));
    offset += sizeof(header->header_size);

    
    memcpy(buffer + offset, &header->file_size, sizeof(header->file_size));
    offset += sizeof(header->file_size);

    
    memcpy(buffer + offset, &header->codeword_map_metadata_size, sizeof(header->codeword_map_metadata_size));
    offset += sizeof(header->codeword_map_metadata_size);

    
    memcpy(buffer + offset, header->codeword_map_metadata, header->codeword_map_metadata_size);
    offset += header->codeword_map_metadata_size;

    return buffer;
}


void Huffman_header_destroy(Huffman_header* header) {
    if (header) {
        if (header->codeword_map_metadata) {
            free(header->codeword_map_metadata);
        }
        free(header);
    }
}








Huffman_header* Huffman_header_deserialize(FILE* file) {
    if (!file) {
        fprintf(stderr, "Invalid file pointer in Huffman_header_deserialize\n");
        return NULL;
    }

    Huffman_header* header = (Huffman_header*)malloc(sizeof(Huffman_header));
    if (!header) {
        perror("Failed to allocate memory for Huffman_header");
        return NULL;
    }

    
    if (fread(&header->magic_number, sizeof(header->magic_number), 1, file) != 1) {
        fprintf(stderr, "Failed to read magic_number\n");
        free(header);
        return NULL;
    }

    
    if (fread(&header->header_size, sizeof(header->header_size), 1, file) != 1) {
        fprintf(stderr, "Failed to read header_size\n");
        free(header);
        return NULL;
    }

    
    if (fread(&header->file_size, sizeof(header->file_size), 1, file) != 1) {
        fprintf(stderr, "Failed to read file_size\n");
        free(header);
        return NULL;
    }

    
    if (fread(&header->codeword_map_metadata_size, sizeof(header->codeword_map_metadata_size), 1, file) != 1) {
        fprintf(stderr, "Failed to read codeword_map_metadata_size\n");
        free(header);
        return NULL;
    }

    
    header->codeword_map_metadata = (uint8_t*)malloc(header->codeword_map_metadata_size);
    if (!header->codeword_map_metadata) {
        perror("Failed to allocate memory for codeword_map_metadata");
        free(header);
        return NULL;
    }

    if (fread(header->codeword_map_metadata, 1, header->codeword_map_metadata_size, file) != header->codeword_map_metadata_size) {
        fprintf(stderr, "Failed to read codeword_map_metadata\n");
        free(header->codeword_map_metadata);
        free(header);
        return NULL;
    }

    return header;
}