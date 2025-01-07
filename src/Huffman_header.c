#include "Huffman_header.h"


// Huffman_header 초기화 함수
Huffman_header* Huffman_header_create(uint32_t magic_number, uint64_t file_size, uint32_t metadata_size, uint8_t* metadata) {
    Huffman_header* header = (Huffman_header*)malloc(sizeof(Huffman_header));
    if (!header) {
        perror("Failed to allocate memory for Huffman_header");
        exit(EXIT_FAILURE);
    }

    header->magic_number = magic_number;
    header->file_size = file_size;
    header->codeword_map_metadata_size = metadata_size;

    // codeword_map_metadata 메모리 복사
    header->codeword_map_metadata = (uint8_t*)malloc(metadata_size);
    if (!header->codeword_map_metadata) {
        perror("Failed to allocate memory for codeword_map_metadata");
        free(header);
        exit(EXIT_FAILURE);
    }
    memcpy(header->codeword_map_metadata, metadata, metadata_size);

    // 헤더 크기 계산
    header->header_size = 4 + 4 + 8 + 4 + metadata_size;

    return header;
}



// Huffman_header 직렬화 함수
uint8_t* Huffman_header_serialize(const Huffman_header* header, size_t* serialized_size) {
    if (!header || !serialized_size) {
        fprintf(stderr, "Invalid arguments to Huffman_header_serialize\n");
        return NULL;
    }

    // 직렬화 크기 계산
    *serialized_size = header->header_size;

    // 직렬화 데이터 버퍼 할당
    uint8_t* buffer = (uint8_t*)malloc(*serialized_size);
    if (!buffer) {
        perror("Failed to allocate memory for serialized Huffman_header");
        exit(EXIT_FAILURE);
    }

    // 직렬화 진행
    size_t offset = 0;

    // magic_number (4 bytes)
    memcpy(buffer + offset, &header->magic_number, sizeof(header->magic_number));
    offset += sizeof(header->magic_number);

    // header_size (4 bytes)
    memcpy(buffer + offset, &header->header_size, sizeof(header->header_size));
    offset += sizeof(header->header_size);

    // file_size (8 bytes)
    memcpy(buffer + offset, &header->file_size, sizeof(header->file_size));
    offset += sizeof(header->file_size);

    // codeword_map_metadata_size (4 bytes)
    memcpy(buffer + offset, &header->codeword_map_metadata_size, sizeof(header->codeword_map_metadata_size));
    offset += sizeof(header->codeword_map_metadata_size);

    // codeword_map_metadata (가변 크기)
    memcpy(buffer + offset, header->codeword_map_metadata, header->codeword_map_metadata_size);
    offset += header->codeword_map_metadata_size;

    return buffer;
}

// Huffman_header 메모리 해제 함수
void Huffman_header_destroy(Huffman_header* header) {
    if (header) {
        if (header->codeword_map_metadata) {
            free(header->codeword_map_metadata);
        }
        free(header);
    }
}


// 멀티 바이트 자료형의 경우 데이터를 쓸때, LSB에 있는 바이트 부터 쓰게 되는데 이러한 방식을 리틀 엔디안이라고 한다.
// 대부분이 리틀 엔디안 방식이다. 
// 문자열 같이 각 자료형이 단일 바이트 인 경우에는 신경 쓸 필요 없다. 앞 바이트부터 쓰게 된다. 
// 특정 자료형을 fwrite로 썼을때, 적절한 위치에서 다시 fread를 하게 되면 해당 자료형 값을 다시 복원됨이 보장된다. 따라서 대개 신경 쓰지 않아도 됨.

// Huffman_header 역직렬화 함수
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

    // magic_number (4 bytes)
    if (fread(&header->magic_number, sizeof(header->magic_number), 1, file) != 1) {
        fprintf(stderr, "Failed to read magic_number\n");
        free(header);
        return NULL;
    }

    // header_size (4 bytes)
    if (fread(&header->header_size, sizeof(header->header_size), 1, file) != 1) {
        fprintf(stderr, "Failed to read header_size\n");
        free(header);
        return NULL;
    }

    // file_size (8 bytes)
    if (fread(&header->file_size, sizeof(header->file_size), 1, file) != 1) {
        fprintf(stderr, "Failed to read file_size\n");
        free(header);
        return NULL;
    }

    // codeword_map_metadata_size (4 bytes)
    if (fread(&header->codeword_map_metadata_size, sizeof(header->codeword_map_metadata_size), 1, file) != 1) {
        fprintf(stderr, "Failed to read codeword_map_metadata_size\n");
        free(header);
        return NULL;
    }

    // codeword_map_metadata (가변 크기)
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