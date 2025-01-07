#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h> 
#include <time.h>
#include "Huffman_tree_util.h"
#include "Huffman_node.h"
#include "Priority_queue.h"
#include "Byte_table.h"
#include "Huffman_header.h"
#include "Trie.h"
#include "Stream_buffer.h"
#include "exception_xmacro.h"

const uint8_t SECTION_DIVIDER[2] = { 0x00, 0x00 };
void compress(const char* inputFilePath);
void decompress(const char* inputFilePath);

int main(int argc, char* argv[]) {
    
    if (argc < 3) {
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_INVALID_INPUT, 
            "Usage: %s <-c | -dc> <input_file>\n", argv[0]);
    }

    const char* mode = argv[1];
    const char* inputFilePath = argv[2];

    if (strcmp(mode, "-c") == 0) {
        
        compress(inputFilePath);
    } else if (strcmp(mode, "-dc") == 0) {
        
        decompress(inputFilePath);
    } else {
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_INVALID_INPUT, 
            "Usage: %s <-c | -dc> <input_file>\n", argv[0]);
    }
    
    return 0;
}


/**
 * @brief Compress a file using the Huffman coding algorithm.
 * 
 * 
 * 1. **FILE READING**:
 *    - Opens the input file in binary mode.
 *    - Reads the file and calculates the frequency of each byte.
 * 
 * 2. **HUFFMAN TREE CONSTRUCTION**:
 *    - Creates a priority queue to build the Huffman tree based on character frequencies.
 *    - Generates a Huffman tree and assigns codewords to each byte.
 * 
 * 3. **HEADER METADATA CREATION & WRITE**:
 *    - Creates a metadata header that contains file size and codeword mapping table.
 *    - Writes the header to the output file.
 * 
 * 4. **COMPRESS ORIGINAL DATA & WRITE**:
 *    - Encodes the input file's contents using the generated Huffman codewords.
 *    - Writes the encoded binary data to the output file.
 * 
 * 5. **RESOURCE CLEANUP**:
 *    - Frees allocated memory and closes all file streams.
 */
void compress(const char* inputFilePath) {
    printf("Running compression...\n");
    clock_t start_time, end_time;
    start_time = clock();
    double elapsed_time;

    // ===== FILE READING =====    
    FILE* inputFile = fopen(inputFilePath, "rb");
    if (!inputFile) {
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FILE_NOT_FOUND, 
            "Failed to open input file: %s\n", inputFilePath);
    }

    char outputFilePath[512]; 
    snprintf(outputFilePath, sizeof(outputFilePath), "%s.huff", inputFilePath);

    FILE* outputFile = fopen(outputFilePath, "wb");
    if (!outputFile) {
        fclose(inputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FILE_NOT_FOUND, 
            "Failed to open output file: %s\n", outputFilePath);
    }

    ByteTable* bt = ByteTable_create();
    if (!bt) {
        fclose(inputFile);
        fclose(outputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FAIL_MEMORY_ALLOCATION, 
            "Failed to create ByteTable.\n");
    }
    uint8_t c;
    uint64_t filesize = 0;
    while (fread(&c, 1, 1, inputFile) == 1) {
        filesize++;
        ByteTable_increment(bt, c);
    }

    
    fseek(inputFile, 0, SEEK_SET);

    // ===== HUFFMAN TREE CONSTRUCTION =====    
    PriorityQueue* pq = Pq_create(256);
    if (!pq) {
        ByteTable_destroy(bt);
        fclose(inputFile);
        fclose(outputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FAIL_MEMORY_ALLOCATION, 
            "Failed to create PriorityQueue.\n");
    }

    for (int i = 0; i < 256; i++) {
        if (bt->table[i] && bt->table[i]->count > 0) {
            Huffman_node* node = Huffman_node_create(i, bt->table[i]->count, NULL, NULL);
            Pq_pushNode(pq, node);
        }
    }

    
    Huffman_node* root = Huffman_tree_generate(pq);
    if (!root) {
        Pq_destroy(pq);
        ByteTable_destroy(bt);
        fclose(inputFile);
        fclose(outputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FAIL_MEMORY_ALLOCATION, 
            "Failed to generate Huffman tree.\n");
    }

    uint8_t code[256];
    Huffman_tree_fill_codewords(root, code, 0, bt);

    // ===== HEADER METADATA CREATION =====
    size_t codewords_metadata_size = 0;
    uint8_t* codewords_metadata = ByteTable_make_codewords_map_metadata(bt, &codewords_metadata_size);

    uint32_t magic_number = 0x46465548; 
    Huffman_header* header = Huffman_header_create(magic_number, filesize, codewords_metadata_size, codewords_metadata);
    
    size_t header_serialized_size = 0;
    uint8_t* header_serialized = Huffman_header_serialize(header, &header_serialized_size);
    
    fwrite(header_serialized, 1, header_serialized_size, outputFile);    
    fwrite(SECTION_DIVIDER, 1, sizeof(SECTION_DIVIDER), outputFile);


    // ===== COMPRESS ORIGINAL DATA & WRITE =====
    size_t input_buffer_size = 1024 * 1024;  
    size_t output_buffer_size = 1024 * 1024; 
    uint8_t* input_buffer = (uint8_t*)malloc(input_buffer_size);
    uint8_t* output_buffer = (uint8_t*)malloc(output_buffer_size);
    if (!input_buffer || !output_buffer) {
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FAIL_MEMORY_ALLOCATION, 
            "Failed to allocate buffers for compression.\n");
    }

    size_t input_bytes_read = 0; 
    size_t output_bit_offset = 0; 
    size_t output_byte_offset = 0; 

    while ((input_bytes_read = fread(input_buffer, 1, input_buffer_size, inputFile)) > 0) {
        for (size_t i = 0; i < input_bytes_read; i++) {
            uint8_t byte = input_buffer[i]; 
            const uint8_t* codeword = bt->table[byte]->codeword;
            size_t codeword_length = strlen((char*)codeword); 

            
            for (size_t j = 0; j < codeword_length; j++) {
                if (codeword[j] == '1') {
                    output_buffer[output_byte_offset] |= (1 << (7 - output_bit_offset));
                }
                output_bit_offset++;

                
                if (output_bit_offset == 8) {
                    output_bit_offset = 0;
                    output_byte_offset++;
                    if (output_byte_offset == output_buffer_size) {
                        fwrite(output_buffer, 1, output_buffer_size, outputFile);
                        memset(output_buffer, 0, output_buffer_size); 
                        output_byte_offset = 0;
                    }
                }
            }
        }
    }

    if (output_bit_offset > 0) {
        output_byte_offset++;
    }
    if (output_byte_offset > 0) {
        fwrite(output_buffer, 1, output_byte_offset, outputFile);
    }


    // ===== RESOURCE CLEANUP =====
    free(input_buffer);
    free(output_buffer);
    free(header_serialized);
    free(codewords_metadata);
    Huffman_header_destroy(header);
    Huffman_node_deallcoate(root);
    Pq_destroy(pq);
    ByteTable_destroy(bt);
    fclose(inputFile);
    fclose(outputFile);

    end_time = clock();
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Compression completed in %.2f seconds. Output written to '%s'.\n", elapsed_time,outputFilePath);
}


/**
 * @brief Decompress a file(*.huff) compressed with the Huffman coding algorithm. outout file's format is '*.orig'.
 * 
 * 1. **FILE READING**:
 *    - Opens the input `.huff` file in binary mode.
 *    - Reads the metadata header to retrieve the original file size and Huffman tree structure.
 * 
 * 2. **HUFFMAN TREE RECONSTRUCTION**:
 *    - ReBuilds a Huffman tree ( Trie structure!! ) from the metadata contained in the header.
 *      
 * 3. **OUTPUT FILE INITILIZATION**:
 *    - Generates a new output file with the original file name (excluding `.huff` extension).
 * 
 * 4.  **DATA DECOMPRESSION**
 *    - load the compressed data to the memory buffer, do Decompressing.
 *    - write decoded binary data to the output file(*.orig)
 * 
 * 5. **Resource Cleanup**:
 *    - Frees allocated memory and closes all file streams.
 */
void decompress(const char* inputFilePath) {
    printf("Running decompression...\n");
    clock_t start_time, end_time;
    start_time = clock();
    double elapsed_time;

    // ===== FILE READING =====
    FILE* inputFile = fopen(inputFilePath, "rb");
    if (!inputFile) {
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FILE_NOT_FOUND, 
            "Failed to open input file: %s\n", inputFilePath);
    }

    
    Huffman_header* header = Huffman_header_deserialize(inputFile);
    if (!header) {
        fclose(inputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_INVALID_FILE, 
            "Failed to deserialize Huffman header.\n");
    }

    // ===== HUFFMAN TREE RECONSTRUCTION =====
    TrieNode* root = Trie_build_from_metadata(header->codeword_map_metadata, header->codeword_map_metadata_size);
    if (!root) {
        Huffman_header_destroy(header);
        fclose(inputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_INVALID_FILE, 
            "Failed to build Trie from metadata.\n");
    }

    
    fseek(inputFile, sizeof(SECTION_DIVIDER), SEEK_CUR);

    
    // =====OUTPUT FILE INITILIZATION=====
    char outputFilePath[512]; 
    strncpy(outputFilePath, inputFilePath, sizeof(outputFilePath) - 1);
    outputFilePath[sizeof(outputFilePath) - 1] = '\0';

    char* extension = strrchr(outputFilePath, '.');
    if (extension && strcmp(extension, ".huff") == 0) {
        *extension = '\0'; 
        strncat(outputFilePath, ".orig", sizeof(outputFilePath) - strlen(outputFilePath) - 1); // add .orig 
    } else {
        fclose(inputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_INVALID_INPUT, 
            "Error: Input file does not have a valid .huff extension.\n");
    }

    FILE* outputFile = fopen(outputFilePath, "wb");
    if (!outputFile) {
        fclose(inputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FILE_NOT_FOUND, 
            "Failed to open output file: %s\n", outputFilePath);
    }

    

    // ===== DATA DECOMPRESSION =====
    fseek(inputFile, 0, SEEK_END);
    long file_size = ftell(inputFile);
    fseek(inputFile, header->header_size + sizeof(SECTION_DIVIDER), SEEK_SET);
    size_t compressed_size = file_size - header->header_size - sizeof(SECTION_DIVIDER);

    Stream_buffer* compressed_data_buffer = Stream_buffer_create(inputFile, 1024 * 1024);
    if (!compressed_data_buffer) {
        Trie_destroy(root);
        Huffman_header_destroy(header);
        fclose(inputFile);
        fclose(outputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FAIL_MEMORY_ALLOCATION, 
            "Failed to allocate memory for compressed data.\n");
    }
    
    size_t total_bits = compressed_size * 8;
    size_t bytes_written = 0;
    TrieNode* current = root;

    for (size_t bit_offset = 0; bit_offset < total_bits; bit_offset++) {
        size_t byte_index = bit_offset / 8;
        size_t bit_index = bit_offset % 8;
        uint8_t bit = (Stream_buffer_get(compressed_data_buffer, byte_index) >> (7 - bit_index)) & 1;

        current = (bit == 0) ? current->left : current->right;

        if (current->is_leaf) {
            fwrite(&current->character, 1, 1, outputFile);
            bytes_written++;
            current = root;

            if (bytes_written == header->file_size) {
                break;
            }
        }
    }

    if (bytes_written != header->file_size) {
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_INVALID_FILE, 
            "Error: Decoded file size (%zu) does not match original file size (%lu)\n",
            bytes_written, header->file_size);
    }



    // ===== RESOURCE CLEANUP =====    
    free(compressed_data_buffer);
    Trie_destroy(root);
    Huffman_header_destroy(header);
    fclose(inputFile);
    fclose(outputFile);

     end_time = clock();
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Decompression completed in %.2f seconds. Output written to '%s'.\n", elapsed_time,outputFilePath);
}
