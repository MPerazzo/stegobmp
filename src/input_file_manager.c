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

void create_output_carrier_file(char *output_file_name, ByteBuffer *carrier_header, ByteBuffer *body) {
    FILE *f = fopen(output_file_name, "w");
    if (f == NULL)
    {
        printf("Error while opening output file!\n");
        exit(1);
    }

    fwrite(carrier_header->start, BYTE, carrier_header->length, f);
    fwrite(body->start, BYTE, body->length, f);

    fclose(f);
}

void create_output_message_file(char *output_file_name, InputFile * message_file)
{
    char * complete_name = strcat(output_file_name, message_file->extension);
    FILE *f = fopen(complete_name, "w");
    if (f == NULL)
    {
        printf("Error while opening output file!\n");
        exit(1);
    }

    fwrite(message_file->file.start, BYTE, message_file->file.length, f);

    fclose(f);
}