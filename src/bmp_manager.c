#include "api.h"
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

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

  fseek(fd, BODY_START_HEADER_OFFSET, SEEK_SET);
  u_int32_t offset;
  fread(&offset, BODY_START_HEADER_SIZE, BYTE, fd);

  header->width = width;
  header->height = height;
  header->bits_per_pixel = bits_per_pixel;
  header->compression = compression;
  header->offset = offset;

  fclose(fd);
  return header;
}

int is_compressed(BMPHeader *header)
{
  return header->compression != 0;
}

/**
 *  get_bmp_body_size
 *
 *  Returns the size of the bmp body in bytes.
 */
int get_bmp_body_size(BMPHeader *header)
{
  return ((int)floor((header->bits_per_pixel * header->width + 31) / 32.0)) * 4 * header->height;
}

ByteBuffer *infer_body(char *filename, BMPHeader *header)
{
  FILE *fd;
  ByteBuffer *byte_buffer = calloc(sizeof(ByteBuffer), 1);

  byte_buffer->length = get_bmp_body_size(header);
  byte_buffer->start = malloc(byte_buffer->length);

  if (byte_buffer->start == NULL)
  {
    return NULL;
  }

  if ((fd = fopen(filename, "rb")) == NULL)
  {
    return NULL;
  }

  fseek(fd, header->offset, SEEK_SET);
  fread(byte_buffer->start, byte_buffer->length, BYTE, fd);

  return byte_buffer;
}