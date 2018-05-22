#include "api.h"
#include <stdlib.h>
#include <string.h>

InputFile *echo_encryption(InputFile *input_file)
{
  input_file->ciphered_file.length = input_file->file.length;
  input_file->ciphered_file.start = calloc(BYTE, input_file->ciphered_file.length);
  memcpy(input_file->ciphered_file.start, input_file->file.start, input_file->ciphered_file.length);
  return input_file;
}

InputFile *echo_decryption(InputFile *input_file)
{
  input_file->file.length = input_file->ciphered_file.length;
  input_file->file.start = calloc(BYTE, input_file->file.length);
  memcpy(input_file->file.start, input_file->ciphered_file.start, input_file->file.length);
  return input_file;
}

InputFile *apply_encryption(InputFile *input_file, EncryptionAlgorithm encryption)
{
  static InputFile *(*fun_ptr_arr[])(InputFile *) = {echo_encryption};
  return (*fun_ptr_arr[encryption])(input_file);
}

InputFile *apply_decryption(InputFile *input_file, EncryptionAlgorithm encryption)
{
  static InputFile *(*dcr_fun_ptr_arr[])(InputFile *) = {echo_encryption};
  return (*dcr_fun_ptr_arr[encryption])(input_file);
}
