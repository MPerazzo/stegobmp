#include <limits.h>
#include <string.h>
#include <stdio.h>

/*
 * Offset and size (in bytes) of the position
 * in the header, where the size of the image
 * (in pixels) can be found.
 */
#define WIDTH_HEADER_OFFSET 0x12
#define WIDTH_HEADER_SIZE 0x4

#define HEIGHT_HEADER_OFFSET 0x16
#define HEIGHT_HEADER_SIZE 0x4

// Amount bits per pixel
#define BITS_PER_PIXEL_HEADER_OFFSET 0x1C
#define BITS_PER_PIXEL_HEADER_SIZE 0x2

// Compression
#define COMPRESSION_HEADER_OFFSET 0x1E
#define COMPRESSION_HEADER_SIZE 0x4

#define BYTE 0x1

int main(int argc, char const *argv[])
{
  FILE *fd;

  if ((fd = fopen("example.bmp", "rb")) == NULL)
  {
    printf("Failed to open file.\n");
    return -1;
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

  printf("Size: %dx%d, Bits per pixel: %d\n", width, height, bits_per_pixel);

  fseek(fd, COMPRESSION_HEADER_OFFSET, SEEK_SET);
  u_int32_t compression;
  fread(&compression, COMPRESSION_HEADER_SIZE, BYTE, fd);

  if (compression == 0)
  {
    printf("File has compression.\n");
  }
  else
  {
    printf("File does not has compression\n");
  }

  fclose(fd);
  return 0;
}
