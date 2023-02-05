#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t byte_t;

typedef struct{
    byte_t* data;
    size_t buffer_size;
    size_t pointer_read;
    size_t pointer_write;

    FILE* handle;

}File_buffer;


File_buffer* init(size_t, FILE*);

int advance(File_buffer*, int, size_t);

byte_t buffer_get(File_buffer*, size_t);

void fbuf_close(File_buffer*);

byte_t* input_dynamic(FILE*, size_t, size_t);

#endif