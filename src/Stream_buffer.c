#include "Stream_buffer.h"


Stream_buffer* Stream_buffer_create(FILE* input_file, size_t buffer_size) {
    Stream_buffer* sb = (Stream_buffer*)malloc(sizeof(Stream_buffer));
    if (!sb) {
        perror("Failed to allocate Stream_buffer");
        exit(EXIT_FAILURE);
    }

    sb->buffer = (uint8_t*)malloc(buffer_size);
    if (!sb->buffer) {
        perror("Failed to allocate buffer for Stream_buffer");
        free(sb);
        exit(EXIT_FAILURE);
    }

    sb->buffer_size = buffer_size;
    sb->start_byte_offset = 0;
    sb->end_byte_offset = 0;
    sb->input_file = input_file;

    // 첫 번째 데이터 읽기
    size_t bytes_read = fread(sb->buffer, 1, buffer_size, input_file);
    if (bytes_read > 0) {
        sb->end_byte_offset = bytes_read; // 바이트 단위로 끝 오프셋 설정
    } else {
        fprintf(stderr, "Failed to read initial data from file\n");
        free(sb->buffer);
        free(sb);
        exit(EXIT_FAILURE);
    }

    return sb;
}


uint8_t Stream_buffer_get(Stream_buffer* sb, size_t byte_index) {
    // 범위 확인
    if (byte_index >= sb->start_byte_offset && byte_index < sb->end_byte_offset) {
        // 버퍼 내에서 해당 바이트 반환
        size_t buffer_index = byte_index - sb->start_byte_offset; // 전체 인덱스 → 버퍼 인덱스
        return sb->buffer[buffer_index];
    } else {
        // 범위를 벗어남: 새로운 데이터를 로드

        // 현재 파일 위치를 기반으로 새 데이터를 읽음
        //size_t bytes_to_skip = byte_index - sb->end_byte_offset;

        // 버퍼 시작과 끝 갱신
        sb->start_byte_offset = byte_index;
        sb->end_byte_offset = sb->start_byte_offset;

        // 파일에서 데이터 읽기
        size_t bytes_read = fread(sb->buffer, 1, sb->buffer_size, sb->input_file);

        if (bytes_read > 0) {
            sb->end_byte_offset = sb->start_byte_offset + bytes_read;
        } else {
            fprintf(stderr, "Failed to read data from file\n");
            exit(EXIT_FAILURE);
        }

        // 새로 로드된 버퍼에서 해당 바이트 반환
        return sb->buffer[0]; // 첫 번째 바이트 반환
    }
}


void Stream_buffer_destroy(Stream_buffer* sb) {
    free(sb->buffer);
    free(sb);
}
