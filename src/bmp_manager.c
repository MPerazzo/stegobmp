#include "api.h"
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

BMPHeader *infer_header(char *filename)
{
  FILE *fd;
  BMPHeader *header = malloc(sizeof(BMPHeader));

  if ((fd = fopen(filename, "rb")) == NULL)
  {
    return NULL;
  }

  fseek(fd, WIDTH_HEADER_OFFSET, SEEK_SET);
  u_int32_t width;
  fread(&width, WIDTH_HEADER_SIZE, BYTE, fd);

  fseek(fd, HEIGHT_HEADER_OFFSET, SEEK_SET);
  u_int32_t height;
  fread(&height, HEIGHT_HEADER_SIZE, BYTE, fd);

  fseek(fd, BITS_PER_PIXEL_HEADER_OFFSET, SEEK_SET);
  u_int16_t bits_per_pixel;
  fread(&bits_per_pixel, BITS_PER_PIXEL_HEADER_SIZE, BYTE, fd);

  fseek(fd, COMPRESSION_HEADER_OFFSET, SEEK_SET);
  u_int32_t compression;
  fread(&compression, COMPRESSION_HEADER_SIZE, BYTE, fd);

  header->width = width;
  header->height = height;
  header->bits_per_pixel = bits_per_pixel;
  header->compression = compression;

  fclose(fd);
  return header;
}

int is_compressed(BMPHeader *header)
{
  return header->compression != 0;
}