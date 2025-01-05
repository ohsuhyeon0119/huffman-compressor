#ifndef STREAM_BUFFER_H
#define STREAM_BUFFER_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct {
    uint8_t* buffer;         
    size_t buffer_size;      
    size_t start_byte_offset; 
    size_t end_byte_offset;   
    FILE* input_file;        
} Stream_buffer;

Stream_buffer* Stream_buffer_create(FILE* input_file, size_t buffer_size) ;

uint8_t Stream_buffer_get(Stream_buffer* sb, size_t byte_index);





# endif