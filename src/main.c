#include <string.h>
#include <stdio.h>
#include "api.h"

int main(int argc, char *argv[])
{
    Options *options = parse_options(argc, argv);
    BMPHeader *header = infer_header(options->input_file_name);

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

    printf("Image data> Size: %dx%d pixels. Bits per pixel: %d. Uses compression: %s.\n", header->width, header->height, header->bits_per_pixel, is_compressed(header) ? "yes" : "no");

    return 0;
}