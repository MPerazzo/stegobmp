#include "api.h"
#include <stdlib.h>
#include <string.h>
#include <byteswap.h>
#include <openssl/evp.h>

#define SUCCESS 1
#define FAILURE !SUCCESS

/**
**  Each encryption function must return a ByteBuffer using the
**  following structure: encryptedData(file data size (4 bytes)|| file data || extension (ending in \0))
*/

ByteBuffer *echo_encryption(InputFile *input_file, EncryptionFunction function, char *password)
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

InputFile *echo_decryption(ByteBuffer *encrypted_file, EncryptionFunction function, char *password)
{

  InputFile *input_file = calloc(BYTE, sizeof(InputFile));

  u_int32_t file_size;
  memcpy(&file_size, encrypted_file->start, 4);

  input_file->file.length = file_size;
  input_file->file.start = calloc(BYTE, input_file->file.length);
  memcpy(input_file->file.start, encrypted_file->start + 4, input_file->file.length);

  u_int32_t extension_length = encrypted_file->length - input_file->file.length - 4;

  input_file->extension = calloc(BYTE, extension_length);
  memcpy(input_file->extension, encrypted_file->start + encrypted_file->length - extension_length, extension_length);
  return input_file;
}

/* For debug:
* Stores the output in a file in base 64
*/
int saveEncryptedData(unsigned char *out, int len, unsigned char *file_name)
{
    BIO *b64;
    BIO *bio;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_file());
    if(bio == NULL)
        return FAILURE;
    if(!BIO_write_filename(bio, file_name))
        return FAILURE;
    bio = BIO_push(b64, bio);
    BIO_write(bio, out, len);
    BIO_flush(bio);
    BIO_free_all(bio);
    return SUCCESS;
}

/*
* 1) Put in buffer Size + Data + Extension
* 2) Encrypt buffer
* 3) Append size of encryption at beggining of data
*/
ByteBuffer *generic_encryption(InputFile *input_file, EncryptionFunction function, char *password)
{

  ByteBuffer * buffer = echo_encryption(input_file, function, password);

  const EVP_CIPHER * (func_ptr[]) = {EVP_aes_128_ecb(), EVP_aes_128_cfb8(), EVP_aes_128_ofb(), EVP_aes_128_cbc(),
    EVP_aes_192_ecb(), EVP_aes_192_cfb8(), EVP_aes_192_ofb(), EVP_aes_192_cbc(),
    EVP_aes_256_ecb(), EVP_aes_256_cfb8(), EVP_aes_256_ofb(), EVP_aes_256_cbc(),
    EVP_des_ecb(), EVP_des_cfb8(), EVP_des_ofb(), EVP_des_cbc()};

  // Encrypt
  uint8_t  * out = calloc(BYTE, buffer->length + 32); // Add extra 256 bits for padding
  unsigned char k[EVP_MAX_KEY_LENGTH];
  unsigned char iv[EVP_MAX_IV_LENGTH];
  const unsigned char *salt = NULL;
  const EVP_MD *dgst = EVP_sha256();

  EVP_BytesToKey(func_ptr[function], dgst, salt, (const unsigned char*)password, strlen(password), 1, k, iv);
  // Init context
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  // Set encryption parameters
  EVP_EncryptInit_ex(ctx, func_ptr[function], NULL, k, iv);
  // Encrypt initial bytes
  int out_length;
  EVP_EncryptUpdate(ctx, out, &out_length, buffer->start, buffer->length);
  // Encrypt remaining block bytes + padding
  int last_block_length;
  EVP_EncryptFinal(ctx, out + out_length, &last_block_length);
  // Clear context
  EVP_CIPHER_CTX_free(ctx);

  // Copy encrypted data to a new buffer and append new size at beggining
  ByteBuffer *encrypted_buffer = calloc(BYTE, sizeof(ByteBuffer));
  encrypted_buffer->length = 4 + out_length + last_block_length;
  encrypted_buffer->start = calloc(BYTE, encrypted_buffer->length);

  u_int32_t total_length = __bswap_32(encrypted_buffer->length);
  memcpy(encrypted_buffer->start, &total_length, 4);
  memcpy(encrypted_buffer->start + 4, out, encrypted_buffer->length - 4);

  /*unsigned char* name = (unsigned char*)"encrypted";
  if (saveEncryptedData(out, out_length + last_block_length, name) == FAILURE)
  {
    fprintf(stderr, "Could not save encrypted file");
    exit(1);
  }*/

  free(buffer->start);
  free(buffer);
  free(out);

  return encrypted_buffer;
}

ByteBuffer *apply_encryption(InputFile *input_file, EncryptionFunction encryption, char *password)
{
  if (encryption == ECHO_FUNCTION)
  {
    return echo_encryption(input_file, encryption, password);
  }
  else
  {
    return generic_encryption(input_file, encryption, password);
  }
}

InputFile *apply_decryption(ByteBuffer *encrypted_file, EncryptionFunction encryption, char *password)
{
  static InputFile *(*dcr_fun_ptr_arr[])(ByteBuffer *, EncryptionFunction, char *) = {echo_decryption};
  return (*dcr_fun_ptr_arr[encryption])(encrypted_file, encryption, password);
}
