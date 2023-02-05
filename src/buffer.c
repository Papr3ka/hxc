#include "buffer.h"

File_buffer* init(size_t size_buf, FILE* handle){
    File_buffer* fbuf = malloc(sizeof(File_buffer));
    fbuf->data = malloc(sizeof(byte_t)*size_buf);
    fbuf->buffer_size = size_buf;
    fbuf->pointer_write = 0;
    fbuf->pointer_read = 0;

    if(!fbuf){
        abort();
    }

    fbuf->handle = handle;

    return fbuf;
}

/*
 * reads bytes into the buffer
 * non-zero value indicates eof
 * value returned indicates number of bytes missed
*/
int advance(File_buffer* fbuf, int bytes_requested, size_t limit){
    fbuf->pointer_read = fbuf->pointer_write;
    while(bytes_requested && !feof(fbuf->handle) && (size_t)ftell(fbuf->handle) <= limit){

        ++fbuf->pointer_write;
        fbuf->pointer_write = fbuf->pointer_write % fbuf->buffer_size;
        fbuf->data[fbuf->pointer_write] = fgetc(fbuf->handle);
        --bytes_requested;

    }

    return bytes_requested;
}

byte_t buffer_get(File_buffer* fbuf, size_t idx){
    return fbuf->data[(fbuf->pointer_read + idx) % fbuf->buffer_size];
}

void fbuf_close(File_buffer* fbuf){
    free(fbuf->data);
    fclose(fbuf->handle);

    free(fbuf);
}

byte_t* input_dynamic(FILE* fbuffer, size_t size, size_t increment){
    byte_t* str;
    str = realloc(NULL, sizeof(*str)*size);
    int ch;

    size_t len = 0;

    if(!str){
        return str;
    }

    while((ch = fgetc(fbuffer)) != '\n'){

        if(ch == EOF){
            exit(1);
        }

        str[len++] = ch;

        if(len >= size){
            str = realloc(str, sizeof(*str)*(size += increment));

            if(!str){
                return str;
            }
        }
    }

    str[len++] = '\0';

    return realloc(str, sizeof(*str)*len);
}