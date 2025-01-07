#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h> 
#include "Huffman_tree_util.h"
#include "Huffman_node.h"
#include "PriorityQueue.h"
#include "Byte_table.h"
#include "Huffman_header.h"
#include "Trie.h"
#include "Stream_buffer.h"
#include "exception_xmacro.h"

const uint8_t SECTION_DIVIDER[2] = { 0x00, 0x00 };

// compress 함수 선언
void compress(const char* inputFilePath);
void decompress(const char* inputFilePath);

int main(int argc, char* argv[]) {
    // 명령줄 인자 확인
    if (argc < 3) {
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_INVALID_INPUT, 
            "Usage: %s <-c | -dc> <input_file>\n", argv[0]);
    }

    const char* mode = argv[1];
    const char* inputFilePath = argv[2];

    if (strcmp(mode, "-c") == 0) {
        // --- Compress Logic ---
        compress(inputFilePath);
    } else if (strcmp(mode, "-dc") == 0) {
        // --- Decompress Logic ---
        decompress(inputFilePath);
    } else {
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_INVALID_INPUT, 
            "Invalid mode. Use -c for compression or -dc for decompression.\n");
    }

    printf("Program finished.\n");
    return 0;
}

void compress(const char* inputFilePath) {
    printf("Running compression...\n");

    // 입력 파일 열기
    FILE* inputFile = fopen(inputFilePath, "rb");
    if (!inputFile) {
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FILE_NOT_FOUND, 
            "Failed to open input file: %s\n", inputFilePath);
    }

    // 출력 파일 이름 생성: 원래 파일명 + ".huff"
    char outputFilePath[512]; // 충분한 크기로 설정
    snprintf(outputFilePath, sizeof(outputFilePath), "%s.huff", inputFilePath);

    FILE* outputFile = fopen(outputFilePath, "wb");
    if (!outputFile) {
        fclose(inputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FILE_NOT_FOUND, 
            "Failed to open output file: %s\n", outputFilePath);
    }

    // ByteTable 생성
    ByteTable* bt = ByteTable_create();
    if (!bt) {
        fclose(inputFile);
        fclose(outputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FAIL_MEMORY_ALLOCATION, 
            "Failed to create ByteTable.\n");
    }

    // 파일 읽기 및 등장 횟수 기록
    uint8_t c;
    uint64_t filesize = 0;
    while (fread(&c, 1, 1, inputFile) == 1) {
        filesize++;
        ByteTable_increment(bt, c);
    }

    // 입력 파일 스트림 재설정
    fseek(inputFile, 0, SEEK_SET);

    // 우선순위 큐 생성
    PriorityQueue* pq = Pq_create(256);
    if (!pq) {
        ByteTable_destroy(bt);
        fclose(inputFile);
        fclose(outputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FAIL_MEMORY_ALLOCATION, 
            "Failed to create PriorityQueue.\n");
    }

    // ByteTable 데이터를 기반으로 HuffmanNode 생성 및 큐에 추가
    for (int i = 0; i < 256; i++) {
        if (bt->table[i] && bt->table[i]->count > 0) {
            Huffman_node* node = Huffman_node_create(i, bt->table[i]->count, NULL, NULL);
            Pq_pushNode(pq, node);
        }
    }

    // 허프만 트리 생성
    Huffman_node* root = Huffman_tree_generate(pq);
    if (!root) {
        Pq_destroy(pq);
        ByteTable_destroy(bt);
        fclose(inputFile);
        fclose(outputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FAIL_MEMORY_ALLOCATION, 
            "Failed to generate Huffman tree.\n");
    }

    // 허프만 트리를 순회하며 ByteTable에 codeword 채우기
    uint8_t code[256];
    Huffman_tree_fill_codewords(root, code, 0, bt);

    // --- DEBUG: ByteTable에 저장된 각 문자와 codeword 출력 ---
    printf("\n[DEBUG] Compress - ByteTable Codewords:\n");
    for (int i = 0; i < 256; i++) {
        if (bt->table[i] && bt->table[i]->codeword) {
            printf("Character '%c' (ASCII: %d): Codeword = %s\n", 
                   (i >= 32 && i <= 126) ? i : '.', i, bt->table[i]->codeword);
        }
    }

    // Codewords Map Metadata 생성
    size_t codewords_metadata_size = 0;
    uint8_t* codewords_metadata = ByteTable_make_codewords_map_metadata(bt, &codewords_metadata_size);

    // 헤더 생성
    uint32_t magic_number = 0x46465548; // "HUFF"
    Huffman_header* header = Huffman_header_create(magic_number, filesize, codewords_metadata_size, codewords_metadata);

    // 헤더 직렬화
    size_t header_serialized_size = 0;
    uint8_t* header_serialized = Huffman_header_serialize(header, &header_serialized_size);

    // 직렬화된 헤더 데이터 파일에 쓰기
    fwrite(header_serialized, 1, header_serialized_size, outputFile);

    // Section Divider 쓰기
    fwrite(SECTION_DIVIDER, 1, sizeof(SECTION_DIVIDER), outputFile);

    // 압축 데이터 쓰기 로직
    size_t input_buffer_size = 1024 * 1024;  // 1MB
    size_t output_buffer_size = 1024 * 1024; // 1MB
    uint8_t* input_buffer = (uint8_t*)malloc(input_buffer_size);
    uint8_t* output_buffer = (uint8_t*)malloc(output_buffer_size);
    if (!input_buffer || !output_buffer) {
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FAIL_MEMORY_ALLOCATION, 
            "Failed to allocate buffers for compression.\n");
    }

    size_t input_bytes_read = 0; // 방금 읽은 fread의 바이트 수
    size_t output_bit_offset = 0; // 출력 버퍼의 비트 오프셋
    size_t output_byte_offset = 0; // 출력 버퍼의 바이트 오프셋

    while ((input_bytes_read = fread(input_buffer, 1, input_buffer_size, inputFile)) > 0) {
        for (size_t i = 0; i < input_bytes_read; i++) {
            uint8_t byte = input_buffer[i]; // 현재 입력 바이트
            const uint8_t* codeword = bt->table[byte]->codeword;
            size_t codeword_length = strlen((char*)codeword); // 코드워드의 길이(비트)

            // 코드워드를 비트 단위로 출력 버퍼에 기록
            for (size_t j = 0; j < codeword_length; j++) {
                if (codeword[j] == '1') {
                    output_buffer[output_byte_offset] |= (1 << (7 - output_bit_offset));
                }
                output_bit_offset++;

                // 1바이트가 가득 차면 다음 바이트로
                if (output_bit_offset == 8) {
                    output_bit_offset = 0;
                    output_byte_offset++;
                    if (output_byte_offset == output_buffer_size) {
                        fwrite(output_buffer, 1, output_buffer_size, outputFile);
                        memset(output_buffer, 0, output_buffer_size); // 버퍼 초기화
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

    printf("Compression completed. Output written to '%s'.\n", outputFilePath);
}

void decompress(const char* inputFilePath) {
    printf("Running decompression...\n");

    // 입력 파일 열기
    FILE* inputFile = fopen(inputFilePath, "rb");
    if (!inputFile) {
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_FILE_NOT_FOUND, 
            "Failed to open input file: %s\n", inputFilePath);
    }

    // 헤더 역직렬화
    Huffman_header* header = Huffman_header_deserialize(inputFile);
    if (!header) {
        fclose(inputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_INVALID_FILE, 
            "Failed to deserialize Huffman header.\n");
    }

    // 트라이 생성
    TrieNode* root = Trie_build_from_metadata(header->codeword_map_metadata, header->codeword_map_metadata_size);
    if (!root) {
        Huffman_header_destroy(header);
        fclose(inputFile);
        THROW_EXCEPTION_AND_EXIT(EXCEPTION_INVALID_FILE, 
            "Failed to build Trie from metadata.\n");
    }

    // 구분자(SECTION_DIVIDER) 건너뛰기
    fseek(inputFile, sizeof(SECTION_DIVIDER), SEEK_CUR);

    // 출력 파일 열기
    char outputFilePath[512]; 
    strncpy(outputFilePath, inputFilePath, sizeof(outputFilePath) - 1);
    outputFilePath[sizeof(outputFilePath) - 1] = '\0';

    char* extension = strrchr(outputFilePath, '.');
    if (extension && strcmp(extension, ".huff") == 0) {
        *extension = '\0';
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

    // 압축 데이터 크기 계산
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

    // 허프만 압축 데이터 디코드
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

    // 자원 해제
    free(compressed_data_buffer);
    Trie_destroy(root);
    Huffman_header_destroy(header);
    fclose(inputFile);
    fclose(outputFile);

    printf("Decompression completed. Output written to '%s'.\n", outputFilePath);
}
