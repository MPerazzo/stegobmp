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

    input_file->file.length = size;
    input_file->file.start = malloc(input_file->file.length);

    fseek(fd, 0, SEEK_SET);
    if (fread(input_file->file.start, 1, input_file->file.length, fd) != (size_t)input_file->file.length)
    {
        return NULL;
    }
    fclose(fd);

    return input_file;
}