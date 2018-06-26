// api.h
#ifndef __API_H
#define __API_H

#include <limits.h>
#include <stdio.h>
#include <sys/types.h>

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

typedef enum steg_algorithm_t {
    LSB1,
    LSB4,
    LSBE
} StegAlgorithm;

typedef enum encryption_algorithm_t {
    ECHO,
    AES128,
    AES192,
    AES256,
    DES
} EncryptionAlgorithm;

typedef enum encryption_mode_t {
    ECB,
    CFB,
    OFB,
    CBC
} EncryptionMode;

// Stores all the parameters of the current execution
typedef struct options_t
{
    Mode mode;
    char *input_file_name;
    char *carrier_file_name;
    char *output_file_name;
    StegAlgorithm steg_algorithm;
    EncryptionAlgorithm encryption_algorithm;
    EncryptionMode encryption_mode;
    char *password;
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

    int _padding_bytes;

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
    u_int32_t length;
} ByteBuffer;

typedef struct pixel_node_t {
    Pixel pixel;
    struct pixel_node_t * next;
} PixelNode;

typedef struct input_file_t
{
    ByteBuffer file;
    ByteBuffer ciphered_file;

    char *extension;
} InputFile;

/********************************
 *
 *  Function definitions
 *
 ********************************/

// Parse command line arguments
Options *parse_options(int argc, char *argv[]);

// Read relevant information from header byte array
BMPHeader *parse_header_from_bytebuffer(ByteBuffer *filename);

// Check wether a bmp file uses compression
int is_compressed(BMPHeader *header);

// Load BMP file body in memory
ByteBuffer *infer_body(char *filename, BMPHeader *header);

// Load BMP file header in memory
ByteBuffer *infer_header(char *filename);

// Inverse operation of infer_header
void write_header_to_bytebuffer(BMPHeader *header, ByteBuffer *byte_buffer);

PixelNode *infer_reversed_pixel_list(BMPHeader *header, ByteBuffer *body);

// Load input file in memory
InputFile *load_file(char *filename);

// Number of bits that the carrier can store
u_int64_t carrier_max_storage(BMPHeader *header, StegAlgorithm steg_algorithm);

// Generates ciphered file for a given encryption algorithm
ByteBuffer *apply_encryption(InputFile *input_file, EncryptionAlgorithm encryption);

// Ofuscates the message into the carrier
PixelNode *steg_apply(ByteBuffer *msg, PixelNode *carrier, StegAlgorithm algorithm);

// Create BMP body from pixel list
ByteBuffer * create_body(BMPHeader *carrier_header, PixelNode *file_with_message);

// Creates a carrier file with a hidden message
void create_output_carrier_file(char *output_file_name, ByteBuffer* carrier_header, ByteBuffer* new_body);

// Generates file from cyphered data
InputFile *apply_decryption(ByteBuffer *encrypted_file, EncryptionAlgorithm encryption);

// Creates the file with the discovered message
void create_output_message_file(char *output_file_name, InputFile * message_file);

// Retrieves the data from the carrier
ByteBuffer *steg_retrieve(PixelNode *carrier, StegAlgorithm algorithm);

#endif