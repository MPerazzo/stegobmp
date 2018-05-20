#include "api.h"
#include <string.h>
#include <stdlib.h>

InputFile *load_file(char* filename)
{

    InputFile *input_file = malloc(sizeof(InputFile));
    
    char * extension = strrchr(filename, '.');
    input_file->extension = extension;

    FILE *fd;

    if ((fd = fopen(filename, "rb")) == NULL)
    {
        return NULL;
    }

    fseek(fd, 0, SEEK_END);
    u_int32_t size = ftell(fd);

    input_file->size = size;

    ByteBuffer *byte_buffer = malloc(sizeof(ByteBuffer));
    byte_buffer->length = size;
    byte_buffer->start = malloc(byte_buffer->length);

    fseek(fd, 0, SEEK_SET);
    fread(byte_buffer->start, 1, byte_buffer->length, fd);
    fclose(fd);

    input_file->file = byte_buffer;

    return input_file;    
}