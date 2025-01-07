#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Byte_table.h"
#include <stdint.h>
#include <math.h> // ceil 사용

// ByteTable 생성
ByteTable* ByteTable_create() {
    ByteTable* bt = (ByteTable*)malloc(sizeof(ByteTable));
    if (!bt) {
        perror("Failed to allocate ByteTable");
        return NULL;
    }

    // 배열 초기화
    for (int i = 0; i < 256; i++) {
        bt->table[i] = NULL;
    }
    return bt;
}

// 특정 바이트의 등장 횟수 증가
void ByteTable_increment(ByteTable* bt, uint8_t byte) {
    if (!bt->table[byte]) {
        // 해당 바이트의 ByteInfo가 없으면 새로 생성
        bt->table[byte] = (ByteInfo*)malloc(sizeof(ByteInfo));
        bt->table[byte]->count = 0;
        bt->table[byte]->codeword = NULL;
    }
    bt->table[byte]->count++;
}

// 특정 바이트의 codeword 설정
void ByteTable_set_codeword(ByteTable* bt, uint8_t byte, const uint8_t* codeword) {
    if (bt->table[byte]) {
        if (bt->table[byte]->codeword) {
            free(bt->table[byte]->codeword); // 기존 코드워드 메모리 해제
        }
        bt->table[byte]->codeword = (uint8_t*) strdup((const char*)codeword); // 새로운 코드워드 설정
    }
}

// ByteTable 출력
void ByteTable_print(ByteTable* bt) {
    printf("\n--- Byte Table ---\n");
    for (int i = 0; i < 256; i++) {
        if (bt->table[i]) {
            printf("Byte '%c' (%d): Count = %lu, Codeword = %s\n",
                   (i >= 32 && i <= 126) ? i : '.', 
                   i, (unsigned long) bt->table[i]->count, 
                   bt->table[i]->codeword ? (char*)(bt->table[i]->codeword) : "(null)");
        }
    }
}

// ByteTable 메모리 해제
void ByteTable_destroy(ByteTable* bt) {
    for (int i = 0; i < 256; i++) {
        if (bt->table[i]) {
            free(bt->table[i]->codeword); // 코드워드 메모리 해제
            free(bt->table[i]);          // ByteInfo 구조체 메모리 해제
        }
    }
    free(bt);
}


// 헷갈려하지 말자. 얘는 메모리 버퍼에 쓰는 것이므로, 리틀 엔디안 방식과 관련이 없다.
uint8_t* ByteTable_make_codewords_map_metadata(ByteTable* bt, size_t* metadata_size) {
    if (!bt || !metadata_size) {
        fprintf(stderr, "Invalid arguments to ByteTable_make_codewords_map_metadata\n");
        return NULL;
    }

    // 1. 필요한 메타데이터 크기 계산
    size_t total_size = 0;
    for (int i = 0; i < 256; i++) {
        if (bt->table[i] && bt->table[i]->codeword) {
            size_t codeword_length = strlen((char*)bt->table[i]->codeword); // 코드워드 길이 (비트 단위)
            total_size += 1;  // target_character (1바이트)
            total_size += 1;  // codeword length (1바이트)
            total_size += (codeword_length + 7) / 8; // codeword 크기 (비트 → 바이트 변환)
        }
    }

    // 2. 메타데이터 버퍼 생성
    uint8_t* metadata = (uint8_t*)malloc(total_size);
    if (!metadata) {
        perror("Failed to allocate memory for codewords_map_metadata");
        exit(EXIT_FAILURE);
    }

    // 3. 메타데이터 생성
    size_t offset = 0;
    for (int i = 0; i < 256; i++) {
        if (bt->table[i] && bt->table[i]->codeword) {
            uint8_t target_character = (uint8_t)i; // 현재 바이트 값
            size_t codeword_length = strlen((char*)bt->table[i]->codeword); // 코드워드 길이

            // target_character 추가
            metadata[offset++] = target_character;

            // codeword length 추가
            metadata[offset++] = (uint8_t)codeword_length;

            // codeword 추가 (비트 단위 → 바이트 단위 변환)
            uint8_t code_buffer = 0;
            int bit_offset = 0;
            for (size_t j = 0; j < codeword_length; j++) {
                if (bt->table[i]->codeword[j] == '1') {
                    code_buffer |= (1 << (7 - bit_offset)); // MSB부터 채움
                }
                bit_offset++;

                if (bit_offset == 8 || j == codeword_length - 1) { // 1바이트 채우거나 마지막 비트인 경우
                    metadata[offset++] = code_buffer;
                    code_buffer = 0;
                    bit_offset = 0;
                }
            }
        }
    }

    // 4. 메타데이터 크기 반환
    *metadata_size = total_size;

    return metadata;
}