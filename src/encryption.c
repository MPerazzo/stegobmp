#include "api.h"
#include <stdlib.h>
#include <string.h>
#include <byteswap.h>

/**
**  Each encryption function must return a ByteBuffer using the
**  following structure: encryptedData(file data size (4 bytes)|| file data || extension (ending in \0))
*/

ByteBuffer *echo_encryption(InputFile *input_file)
{

  ByteBuffer * buffer = calloc(BYTE, sizeof(ByteBuffer));

  int extension_size = strlen(input_file->extension);
  // 4 bytes for the size + file data + extension + 1 extra byte for \0
  buffer->length = 4 + input_file->file.length + extension_size + 1;
  buffer->start = calloc(BYTE, buffer->length);

  // Save file size in big endian
  u_int32_t file_size = __bswap_32(input_file->file.length);

  // Convert input file into a ByteBuffer using its respective offsets
  memcpy(buffer->start, &file_size, 4);
  memcpy(buffer->start + 4, input_file->file.start, input_file->file.length);
  memcpy(buffer->start + 4 + input_file->file.length, input_file->extension, extension_size + 1); // Copy extension & \0

  return buffer;
}

InputFile *echo_decryption(ByteBuffer *encrypted_file)
{

  InputFile *input_file = malloc(sizeof(InputFile));

  u_int32_t file_size;
  memcpy(encrypted_file->start, &file_size, 4);

  input_file->file.length = __bswap_32(file_size);
  memcpy(input_file->file.start, encrypted_file->start + 4, input_file->file.length);

  u_int32_t extension_length = encrypted_file->length - input_file->file.length - 4;

  memcpy(input_file->extension, encrypted_file->start + encrypted_file->length - extension_length, extension_length);
  return input_file;
}

ByteBuffer *apply_encryption(InputFile *input_file, EncryptionAlgorithm encryption)
{
  static ByteBuffer *(*fun_ptr_arr[])(InputFile *) = {echo_encryption};
  return (*fun_ptr_arr[encryption])(input_file);
}

InputFile *apply_decryption(ByteBuffer *encrypted_file, EncryptionAlgorithm encryption)
{
  static InputFile *(*dcr_fun_ptr_arr[])(ByteBuffer *) = {echo_decryption};
  return (*dcr_fun_ptr_arr[encryption])(encrypted_file);
}
