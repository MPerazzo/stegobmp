#include "api.h"
#include <string.h>
#include <stdlib.h>
#include <byteswap.h>

/*
**  We assume that if any of these functions is called
**  there is enough space to ofuscate the message.
*/

PixelNode *LSB1_apply(ByteBuffer *msg, PixelNode *carrier)
{

  int pixel_index = 0;
  PixelNode *pixel_node = carrier;
  u_int8_t component[] = {pixel_node->pixel.blue, pixel_node->pixel.green, pixel_node->pixel.red};

  for (u_int32_t msg_offset = 0; msg_offset < msg->length; msg_offset++)
  {
    u_int8_t byte;
    memcpy(&byte, msg->start + msg_offset, 1);
    for (int j = 7; j >= 0; j--)
    {
        // Get bit from msg
        u_int8_t bit = (byte >> j) & 1; // 0000000x;

        // Hide bit in LSB1 mode
        component[pixel_index] = (component[pixel_index] & 0xFE) | bit;
        pixel_index++;

      if (pixel_index == PIXEL_SIZE)
      {
        // Save new data
        pixel_node->pixel.blue = component[0];
        pixel_node->pixel.green = component[1];
        pixel_node->pixel.red = component[2];

        // Prepare next pixel
        pixel_node = pixel_node->next; // Next pixel
        pixel_index = 0;
        component[0] = pixel_node->pixel.blue;
        component[1] = pixel_node->pixel.green;
        component[2] = pixel_node->pixel.red;
      }
    }
  }

  // Save the remaining data
  switch (pixel_index)
  {
    case 1:
      pixel_node->pixel.blue = component[0];
      break;
    case 2:
      pixel_node->pixel.blue = component[0];
      pixel_node->pixel.green = component[1];
      break;
  }

  return carrier;
}

PixelNode *LSB4_apply(ByteBuffer *msg, PixelNode *carrier)
{
  int pixel_index = 0;
  PixelNode *pixel_node = carrier;
  u_int8_t component[] = {pixel_node->pixel.blue, pixel_node->pixel.green, pixel_node->pixel.red};

  for (u_int32_t msg_offset = 0; msg_offset < msg->length; msg_offset++)
  {
    u_int8_t byte;
    memcpy(&byte, msg->start + msg_offset, BYTE);
    for (int j = 1; j >= 0; j--)
    {
        // Get 4 bits from msg
        u_int8_t four_bits = (byte >> (j*4)) & 0x0F; // 0000xxxx;

        // Hide 4 bits in LSB4 mode
        component[pixel_index] = (component[pixel_index] & 0xF0) | four_bits;
        pixel_index++;

      if (pixel_index == PIXEL_SIZE)
      {
        // Save new data
        pixel_node->pixel.blue = component[0];
        pixel_node->pixel.green = component[1];
        pixel_node->pixel.red = component[2];

        // Prepare next pixel
        pixel_node = pixel_node->next; // Next pixel
        pixel_index = 0;
        component[0] = pixel_node->pixel.blue;
        component[1] = pixel_node->pixel.green;
        component[2] = pixel_node->pixel.red;
      }
    }
  }

    // Save the remaining data
  switch (pixel_index)
  {
    case 1:
      pixel_node->pixel.blue = component[0];
      break;
    case 2:
      pixel_node->pixel.blue = component[0];
      pixel_node->pixel.green = component[1];
      break;
  }

  return carrier;
}

PixelNode *LSBE_apply(ByteBuffer *msg, PixelNode *carrier)
{
  int pixel_index = 0;
  PixelNode *pixel_node = carrier;
  u_int8_t component[] = {pixel_node->pixel.blue, pixel_node->pixel.green, pixel_node->pixel.red};

  for (u_int32_t msg_offset = 0; msg_offset < msg->length; msg_offset++)
  {
    u_int8_t byte;
    memcpy(&byte, msg->start + msg_offset, 1);
    for (int j = 7; j >= 0; j--)
    {
        // Get bit from msg
        u_int8_t bit = (byte >> j) & 1; // 0000000x;

        // Move current pixel index to a pixel in which it's possible to write data
        while (component[pixel_index] != 0xFE || component[pixel_index] != 0xFF ) {
          pixel_index++;
          if (pixel_index == PIXEL_SIZE)
          {
            // Save new data
            pixel_node->pixel.blue = component[0];
            pixel_node->pixel.green = component[1];
            pixel_node->pixel.red = component[2];

            // Prepare next pixel
            pixel_node = pixel_node->next; // Next pixel
            pixel_index = 0;
            component[0] = pixel_node->pixel.blue;
            component[1] = pixel_node->pixel.green;
            component[2] = pixel_node->pixel.red;
          }
        }

        // Hide bit in LSBE mode
        component[pixel_index] = (component[pixel_index] & 0xFE) | bit;
        pixel_index++;

      if (pixel_index == PIXEL_SIZE)
      {
        // Save new data
        pixel_node->pixel.blue = component[0];
        pixel_node->pixel.green = component[1];
        pixel_node->pixel.red = component[2];

        // Prepare next pixel
        pixel_node = pixel_node->next; // Next pixel
        pixel_index = 0;
        component[0] = pixel_node->pixel.blue;
        component[1] = pixel_node->pixel.green;
        component[2] = pixel_node->pixel.red;
      }
    }
  }

  // Save the remaining data
  switch (pixel_index)
  {
    case 1:
      pixel_node->pixel.blue = component[0];
      break;
    case 2:
      pixel_node->pixel.blue = component[0];
      pixel_node->pixel.green = component[1];
      break;
  }

  return carrier;
}

ByteBuffer *LSB1_retrieve(PixelNode *carrier, int encrypted)
{

  // 4 bytes del tamaño
  // Data de longitud dada por los 4 bytes
  // extension que termina en \0

  ByteBuffer * buffer = calloc(BYTE, sizeof(ByteBuffer));

  u_int32_t size = 0;
  PixelNode * curr = carrier;

  // Retrieve the first 32 bits containing the size
  int curr_bits_retrieved = 0;
  int curr_index = 0;

  u_int8_t component[] = {curr->pixel.blue, curr->pixel.green, curr->pixel.red};
  while (curr_bits_retrieved < 32)
  {
    size <<= 1;
    size = size | (component[curr_index] & 0x1);
    curr_bits_retrieved++;
    curr_index++;

    if (curr_index == PIXEL_SIZE)
    {
      curr_index = 0;
      curr = curr->next;
      component[0] = curr->pixel.blue;
      component[1] = curr->pixel.green;
      component[2] = curr->pixel.red;
    }
  }

  buffer->length = size + 4;
  buffer->start = calloc(BYTE, buffer->length);
  memcpy(buffer->start, &size, 4);

  if (encrypted)
  {
    //There's an extra bit in the current pixel without reading that we will manually inject
    u_int8_t extra_bit = curr->pixel.red & 0x1;
    curr = curr->next;

    curr_index = 0;
    curr_bits_retrieved = 1;
    component[0] = curr->pixel.blue;
    component[1] = curr->pixel.green;
    component[2] = curr->pixel.red;

    u_int8_t * current = buffer->start + 4;
    // Inject exta bit
    *current = extra_bit;
    while (curr_bits_retrieved < (int)size * 8)
    {

      (*current) <<= 1;
      *current = *current | (component[curr_index] & 0x1);

      curr_bits_retrieved++;
      curr_index++;

      if (curr_index == PIXEL_SIZE)
      {
        curr_index = 0;
        curr = curr->next;
        component[0] = curr->pixel.blue;
        component[1] = curr->pixel.green;
        component[2] = curr->pixel.red;
      }

      if (curr_bits_retrieved % 8 == 0)
      {
        current += BYTE;
      }
    }

    return buffer;
  }

  //There's an extra bit in the current pixel without reading that we will manually inject
  u_int8_t extra_bit = curr->pixel.red & 0x1;
  curr = curr->next;

  curr_index = 0;
  curr_bits_retrieved = 1;
  component[0] = curr->pixel.blue;
  component[1] = curr->pixel.green;
  component[2] = curr->pixel.red;

  u_int8_t * current = buffer->start + 4;
  // Inject exta bit
  *current = extra_bit;
  while (curr_bits_retrieved < (int)size * 8)
  {

    (*current) <<= 1;
    *current = *current | (component[curr_index] & 0x1);

    curr_bits_retrieved++;
    curr_index++;

    if (curr_index == PIXEL_SIZE)
    {
      curr_index = 0;
      curr = curr->next;
      component[0] = curr->pixel.blue;
      component[1] = curr->pixel.green;
      component[2] = curr->pixel.red;
    }

    if (curr_bits_retrieved % 8 == 0)
    {
      current += BYTE;
    }
  }

  // TODO: DO ONLY IF NO ENCRYPTION IS NEEDED

  char extension[100];

  switch (curr_index)
  {
    case 0:
      curr_bits_retrieved = 0;
      break;
    case 1:
      extension[0] = ((curr->pixel.green & 0x1) << 1) | (curr->pixel.red & 0x1);
      curr = curr->next;
      curr_bits_retrieved = 2;
      break;
    case 2:
      extension[0] = curr->pixel.red & 0x1;
      curr = curr->next;
      curr_bits_retrieved = 1;
      break;
  }

  // The magic
  curr_index = 0;
  char stop_found = 0;
  int i = 0;
  component[0] = curr->pixel.blue;
  component[1] = curr->pixel.green;
  component[2] = curr->pixel.red;

  while (!stop_found)
  {

    extension[i] <<= 1;
    extension[i] = extension[i] | (component[curr_index] & 0x1);

    curr_bits_retrieved++;
    curr_index++;

    if (curr_index == PIXEL_SIZE)
    {
      curr_index = 0;
      curr = curr->next;
      component[0] = curr->pixel.blue;
      component[1] = curr->pixel.green;
      component[2] = curr->pixel.red;
    }

    if (curr_bits_retrieved % 8 == 0)
    {
      if (extension[i] == '\0')
      {
        // Hurray \o/
        stop_found = 1;
      }
      i++;
    }
  }

  int extension_length = strlen(extension) + 1;
  buffer->start = realloc(buffer->start, buffer->length + extension_length);
  memcpy(buffer->start + buffer->length, extension, extension_length);
  buffer->length += extension_length;

  return buffer;
}

ByteBuffer *LSB4_retrieve(PixelNode *carrier, int encrypted)
{
  ByteBuffer * buffer = calloc(BYTE, sizeof(ByteBuffer));

  u_int32_t size = 0;
  PixelNode * curr = carrier;

  // Retrieve the first 32 bits containing the size
  int curr_bits_retrieved = 0;
  int curr_index = 0;

  u_int8_t component[] = {curr->pixel.blue, curr->pixel.green, curr->pixel.red};
  while (curr_bits_retrieved < 32)
  {
    size <<= 4;
    size = size | (component[curr_index] & 0xF);
    curr_bits_retrieved += 4;
    curr_index++;

    if (curr_index == PIXEL_SIZE)
    {
      curr_index = 0;
      curr = curr->next;
      component[0] = curr->pixel.blue;
      component[1] = curr->pixel.green;
      component[2] = curr->pixel.red;
    }
  }

  buffer->length = size + 4;
  buffer->start = calloc(BYTE, buffer->length);
  memcpy(buffer->start, &size, 4);

  //There's an extra bit in the current pixel without reading that we will manually inject
  u_int8_t extra_bit = curr->pixel.red & 0x4;
  curr = curr->next;

  curr_index = 0;
  curr_bits_retrieved = 4;
  component[0] = curr->pixel.blue;
  component[1] = curr->pixel.green;
  component[2] = curr->pixel.red;

  u_int8_t * current = buffer->start + 4;
  // Inject exta bit
  *current = extra_bit;
  while (curr_bits_retrieved < (int)size * 8)
  {

    (*current) <<= 4;
    *current = *current | (component[curr_index] & 0xF);

    curr_bits_retrieved += 4;
    curr_index++;

    if (curr_index == PIXEL_SIZE)
    {
      curr_index = 0;
      curr = curr->next;
      component[0] = curr->pixel.blue;
      component[1] = curr->pixel.green;
      component[2] = curr->pixel.red;
    }

    if (curr_bits_retrieved % 8 == 0)
    {
      current += BYTE;
    }
  }

  // TODO: DO ONLY IF NO ENCRYPTION IS NEEDED

  char extension[100];

  switch (curr_index)
  {
    case 0:
      curr_bits_retrieved = 0;
      break;
    case 1:
      extension[0] = ((curr->pixel.green & 0xF) << 4) | (curr->pixel.red & 0x4);
      curr = curr->next;
      curr_bits_retrieved = 8;
      break;
    case 2:
      extension[0] = curr->pixel.red & 0x4;
      curr = curr->next;
      curr_bits_retrieved = 4;
      break;
  }

  // The magic
  curr_index = 0;
  char stop_found = 0;
  int i = 0;
  component[0] = curr->pixel.blue;
  component[1] = curr->pixel.green;
  component[2] = curr->pixel.red;

  while (!stop_found)
  {

    extension[i] <<= 4;
    extension[i] = extension[i] | (component[curr_index] & 0xF);

    curr_bits_retrieved += 4;
    curr_index++;

    if (curr_index == PIXEL_SIZE)
    {
      curr_index = 0;
      curr = curr->next;
      component[0] = curr->pixel.blue;
      component[1] = curr->pixel.green;
      component[2] = curr->pixel.red;
    }

    if (curr_bits_retrieved % 8 == 0)
    {
      if (extension[i] == '\0')
      {
        // Hurray \o/
        stop_found = 1;
      }
      i++;
    }
  }

  int extension_length = strlen(extension) + 1;
  buffer->start = realloc(buffer->start, buffer->length + extension_length);
  memcpy(buffer->start + buffer->length, extension, extension_length);
  buffer->length += extension_length;

  return buffer;
}

ByteBuffer *LSBE_retrieve(PixelNode *carrier, int encrypted)
{
  ByteBuffer * buffer = calloc(BYTE, sizeof(ByteBuffer));

  u_int32_t size = 0;
  PixelNode * curr = carrier;

  // Retrieve the first 32 bits containing the size
  int curr_bits_retrieved = 0;
  int curr_index = 0;

  u_int8_t component[] = {curr->pixel.blue, curr->pixel.green, curr->pixel.red};
  while (curr_bits_retrieved < 32)
  {
    if (component[curr_index] == 0xFE || component[curr_index] == 0xFF)
    {
      size <<= 1;
      size = size | (component[curr_index] & 0x1);
      curr_bits_retrieved++;
    }
    curr_index++;

    if (curr_index == PIXEL_SIZE)
    {
      curr_index = 0;
      curr = curr->next;
      component[0] = curr->pixel.blue;
      component[1] = curr->pixel.green;
      component[2] = curr->pixel.red;
    }
  }

  u_int8_t extra_bit = 0;
  int extra_bit_length = 0;
  switch (curr_index)
  {
    case 0:
      break;
    case 1:
      // falta el green y el red (si es que cumple)
      if (curr->pixel.green == 0xFE || curr->pixel.green == 0xFF)
      {
        extra_bit_length++;
        extra_bit = curr->pixel.green & 0x1;
      }
      if (curr->pixel.red == 0xFE || curr->pixel.red == 0xFF)
      {
        extra_bit_length++;
        extra_bit <<= 1;
        extra_bit = curr->pixel.red & 0x1;
      }
      curr = curr->next;
      break;
    case 2:
      // falta el red (si es que cumple)
      if (curr->pixel.red == 0xFE || curr->pixel.red == 0xFF)
      {
        extra_bit_length++;
        extra_bit = curr->pixel.red & 0x1;
      }
      curr = curr->next;
      break;
  }

  buffer->length = size + 4;
  buffer->start = calloc(BYTE, buffer->length);
  memcpy(buffer->start, &size, 4);

  if (encrypted)
  {
    curr_index = 0;
    curr_bits_retrieved = extra_bit_length;
    component[0] = curr->pixel.blue;
    component[1] = curr->pixel.green;
    component[2] = curr->pixel.red;

    u_int8_t * current = buffer->start + 4;
    // Inject exta bit
    *current = extra_bit;
    while (curr_bits_retrieved < (int)size * 8)
    {
      if (component[curr_index] == 0xFE | component[curr_index] == 0xFF)
      {
        (*current) <<= 1;
        *current = *current | (component[curr_index] & 0x1);

        curr_bits_retrieved++;
      }
      curr_index++;

      if (curr_index == PIXEL_SIZE)
      {
        curr_index = 0;
        curr = curr->next;
        component[0] = curr->pixel.blue;
        component[1] = curr->pixel.green;
        component[2] = curr->pixel.red;
      }

      if (curr_bits_retrieved % 8 == 0)
      {
        current += BYTE;
      }
    }

    return buffer;
  }

  //There's an extra bit in the current pixel without reading that we will manually inject
  // u_int8_t extra_bit = curr->pixel.red & 0x1;
  // curr = curr->next;

  curr_index = 0;
  curr_bits_retrieved = extra_bit_length;
  component[0] = curr->pixel.blue;
  component[1] = curr->pixel.green;
  component[2] = curr->pixel.red;

  u_int8_t * current = buffer->start + 4;
  // Inject exta bit
  *current = extra_bit;

  while (curr_bits_retrieved < (int)size * 8)
  {
    if (component[curr_index] == 0xFE || component[curr_index] == 0xFF)
    {
      (*current) <<= 1;
      *current = *current | (component[curr_index] & 0x1);

      curr_bits_retrieved++;
    }
    curr_index++;

    if (curr_index == PIXEL_SIZE)
    {
      curr_index = 0;
      curr = curr->next;
      component[0] = curr->pixel.blue;
      component[1] = curr->pixel.green;
      component[2] = curr->pixel.red;
    }

    if (curr_bits_retrieved % 8 == 0)
    {
      current += BYTE;
    }
  }

  // TODO: DO ONLY IF NO ENCRYPTION IS NEEDED
  char extension[100];

  curr_bits_retrieved = 0;
  switch (curr_index)
  {
    case 0:
      curr_bits_retrieved = 0;
      break;
    case 1:
      if (curr->pixel.green == 0xFE || curr->pixel.green == 0xFF)
      {
        curr_bits_retrieved++;
        extension[0] = curr->pixel.green & 0x1;
      }
      if (curr->pixel.red == 0xFE || curr->pixel.red == 0xFF)
      {
        curr_bits_retrieved++;
        extension[0] <<= 1;
        extension[0] = extension[0] | (curr->pixel.red & 0x1);
      }
      curr = curr->next;
      break;
    case 2:
      if (curr->pixel.red == 0xFE || curr->pixel.red == 0xFF)
      {
        curr_bits_retrieved++;
        extension[0] = curr->pixel.red & 0x1;
      }
      curr = curr->next;
      break;
  }

  // The magic
  curr_index = 0;
  char stop_found = 0;
  int i = 0;
  component[0] = curr->pixel.blue;
  component[1] = curr->pixel.green;
  component[2] = curr->pixel.red;

  while (!stop_found)
  {
    if (component[curr_index] == 0xFE || component[curr_index] == 0xFF)
    {
      extension[i] <<= 1;
      extension[i] = extension[i] | (component[curr_index] & 0x1);

      curr_bits_retrieved++;
    }
    curr_index++;

    if (curr_index == PIXEL_SIZE)
    {
      curr_index = 0;
      curr = curr->next;
      component[0] = curr->pixel.blue;
      component[1] = curr->pixel.green;
      component[2] = curr->pixel.red;
    }

    if (curr_bits_retrieved % 8 == 0)
    {
      if (extension[i] == '\0')
      {
        // Hurray \o/
        stop_found = 1;
      }
      i++;
    }
  }

  int extension_length = strlen(extension) + 1;
  buffer->start = realloc(buffer->start, buffer->length + extension_length);
  memcpy(buffer->start + buffer->length, extension, extension_length);
  buffer->length += extension_length;

  return buffer;
}


PixelNode *steg_apply(ByteBuffer *msg, PixelNode *carrier, StegAlgorithm algorithm)
{
  static PixelNode *(*alg_appl_ptr_arr[])(ByteBuffer *, PixelNode *) = {LSB1_apply, LSB4_apply, LSBE_apply};
  return (*alg_appl_ptr_arr[algorithm])(msg, carrier);
}

ByteBuffer *steg_retrieve(PixelNode *carrier, StegAlgorithm algorithm, int encrypted)
{
  static ByteBuffer *(*alg_rtrv_ptr_arr[])(PixelNode *, int) = {LSB1_retrieve, LSB4_retrieve, LSBE_retrieve};
  return (*alg_rtrv_ptr_arr[algorithm])(carrier, encrypted);
}