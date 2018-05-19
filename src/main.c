#include <string.h>
#include <stdio.h>
#include "api.h"

#define DEBUG 1

int main(int argc, char *argv[])
{
    Options *options = parse_options(argc, argv);
    BMPHeader *header = infer_header(options->carrier_file_name);

    if (header == NULL)
    {
        fprintf(stderr, "Could not load file.\n");
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

    // Debug
    if (DEBUG)
    {
        printf("Image data> Size: %dx%d pixels. Bits per pixel: %d. Uses compression: %s.\n", header->width, header->height, header->bits_per_pixel, is_compressed(header) ? "yes" : "no");
        printf("Body data> Length: %d\n", carrier_body->length);
    }

    //TODO: Move this
    free(options);
    free(header);
    free(carrier_body->start);
    free(carrier_body);

    return 0;
}