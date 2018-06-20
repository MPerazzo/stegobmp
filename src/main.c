#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "api.h"

#define DEBUG 1

int main(int argc, char *argv[])
{
    Options *options = parse_options(argc, argv);
    ByteBuffer *carrier_header = infer_header(options->carrier_file_name);
    if (carrier_header == NULL)
    {
        fprintf(stderr, "Could not load carrier header in memory.\n");
        return 1;
    }

    BMPHeader *header = parse_header_from_bytebuffer(carrier_header);

    if (header == NULL)
    {
        fprintf(stderr, "Could not parse header from byte buffer.\n");
        return 1;
    }

    if (header->compression != 0)
    {
        fprintf(stderr, "Input file must not use compression.\n");
        return 1;
    }

    ByteBuffer *carrier_body = infer_body(options->carrier_file_name, header);

    if (carrier_body == NULL)
    {
        fprintf(stderr, "Error trying to infer BMP body.\n");
        return 1;
    }

    PixelNode * list = infer_reversed_pixel_list(header, carrier_body);

    //TODO: This is throwing segmentation fault if there's no input name
    InputFile *input_file = load_file(options->input_file_name);

    if (input_file == NULL){
        fprintf(stderr, "Error trying to read input file.\n");
        return 1;
    }

    u_int8_t extension_length = 1;
    if (input_file->extension != NULL)
    {
        extension_length += strlen(input_file->extension);
    }

    u_int64_t max_storage = carrier_max_storage(header, options->steg_algorithm);
    u_int64_t total_data = (4 + input_file->file.length + extension_length) * 8;

    if (total_data > max_storage){
        fprintf(stderr, "BMP carrier is not big enough to save input file. ");
        fprintf(stderr, "Max Capacity: %d bytes\n", (int) (max_storage/8.0));
        return 1;
    }

    InputFile * encrypted_file = apply_encryption(input_file, options->encryption_algorithm);

    // Debug
    if (DEBUG)
    {
        printf("Image data> Size: %dx%d pixels. Bits per pixel: %d. Uses compression: %s.\n", header->width, header->height, header->bits_per_pixel, is_compressed(header) ? "yes" : "no");
        printf("Body data> Length: %d\n", carrier_body->length);
        printf("Header data> Length: %d\n", carrier_header->length);
        printf("Total size: %d\n", carrier_header->length + carrier_body->length);

        printf("%d, %d, %d\n",list->pixel.red, list->pixel.green, list->pixel.blue);
        list = list->next->next->next->next->next->next->next->next->next->next->next->next;
        printf("%d, %d, %d\n",list->pixel.red, list->pixel.green, list->pixel.blue);
        printf("Input file> Size: %d Extension: %s\n", input_file->file.length, input_file->extension);
        printf("Encrypted file> Size %d\n", encrypted_file->ciphered_file.length);
    }




    //TODO: Move this
    free(options);
    free(header);
    free(carrier_header->start);
    free(carrier_header);
    free(carrier_body->start);
    free(carrier_body);
    free(input_file->file.start);
    free(input_file);

    while (list != NULL)
    {
        PixelNode * next = list->next;
        free(list);
        list = next;
    }

    return 0;
}