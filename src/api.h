// api.h
#ifndef __API_H
#define __API_H

#include <limits.h>
#include <stdio.h>

/********************************
 *
 *  Macro definitions
 *
 ********************************/

// Size of a byte
#define BYTE 0x1

/*
 * Offsets & sizes (in bytes) of the BMP header
 * where useful information can be found.
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

#define BODY_START_HEADER_OFFSET 0x0A
#define BODY_START_HEADER_SIZE 0x4

// Size of a pixel in bytes
#define PIXEL_SIZE 3

/********************************
 *
 *  Struct definitions
 *
 ********************************/

// Possible modes of execution
typedef enum mode_t
{
    EMBED,
    EXTRACT
} Mode;

// Stores all the parameters of the current execution
typedef struct options_t
{
    Mode mode;
    char *input_file_name;
    char *carrier_file_name;
    char *output_file_name;
} Options;

// Stores all the relevant BMP header information
typedef struct header_t
{
    // Size in pixels
    u_int32_t width;
    u_int32_t height;

    u_int16_t bits_per_pixel;

    // Has compression unless value is 0
    u_int32_t compression;

    // The location of the start of the image information relative to the start of the file
    u_int32_t offset;
} BMPHeader;

typedef struct pixel_t
{
    u_int8_t green;
    u_int8_t blue;
    u_int8_t red;
} Pixel;

typedef struct byte_buffer_t
{
    u_int8_t *start;
    int length;
} ByteBuffer;

/********************************
 *
 *  Function definitions
 *
 ********************************/

// Parse command line arguments
Options *parse_options(int argc, char *argv[]);

// Read relevant information from header
BMPHeader *infer_header(char *filename);

// Check wether a bmp file uses compression
int is_compressed(BMPHeader *header);

// Load BMP file body in memory
ByteBuffer *infer_body(char *filename, BMPHeader *header);

#endif