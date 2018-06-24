#include "api.h"
#include <string.h>

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

      if (pixel_index == 3)
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

  return carrier;
}

PixelNode *LSB4_apply(ByteBuffer *msg, PixelNode *carrier)
{
  printf("TODO: Implement :)\n");
  return NULL;
}

PixelNode *LSBE_apply(ByteBuffer *msg, PixelNode *carrier)
{
  printf("TODO: Implement :)\n");
  return NULL;
}

ByteBuffer *LSB1_retrieve(PixelNode *carrier)
{

  // 4 bytes del tamaño
  // Data de longitud dada por los 4 bytes
  // extension que termina en \0

  ByteBuffer * buffer = calloc(BYTE, sizeof(ByteBuffer));

  u_int32_t size;
  PixelNode * curr = carrier:

  // Retrieve the first 32 bits containing the size
  int curr_bits_retrieved = 0;
  int curr_index = 0;

  u_int8_t component[] = {curr->pixel.blue, curr->pixel.green, curr->pixel.red};
  while (curr_bits_retrieved < 32)
    size <<= 1;
    size = size | (component[curr_index] & 0b1);
    curr_bits_retrieved++;
    curr_index++;

    if (curr_index == PIXEL_SIZE){
      curr_index = 0;
      curr = curr->next;
      component[0] = curr->pixel.blue;
      component[1] = curr->pixel.green;
      component[2] = curr->pixel.red;
    }
  }

  buffer->length = size;
  buffer->start = calloc(BYTE, buffer->length);

  //There's an extra bit in the current pixel without reading that we will manually inject
  u_int8_t extra_bit = curr->pixel.red & 0b1;
  curr = curr->next;

  curr_index = 0;
  curr_bits_retrieved = 1;
  component[0] = curr->pixel.blue;
  component[1] = curr->pixel.green;
  component[2] = curr->pixel.red;

  u_int8_t * current = buffer->start;
  // Inject exta bit
  *current = extra_bit;
  while (curr_bits_retrieved < size * 8) {

    (*current) <<= 1:
    *current = *current | (component[curr_index] & 0b1);

    curr_bits_retrieved++;
    curr_index++;

    if (curr_index == PIXEL_SIZE){
      curr_index = 0;
      curr = curr->next;
      component[0] = curr->pixel.blue;
      component[1] = curr->pixel.green;
      component[2] = curr->pixel.red;
    }

    if (curr_bits_retrieved % 8 == 0) {
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
      extension[0] = ((curr->pixel.green & 0b1) << 1) | (curr->pixel.red & 0b1);
      curr = curr->next;
      curr_bits_retrieved = 2;
      break;
    case 2:
      extension[0] = curr->pixel.red & 0b1;
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

  while (!stop_found){

    extension[i] <<= 1;
    extension[i] = extension[i] | (component[curr_index] & 0b1);

    curr_bits_retrieved++;
    curr_index++;

    //falta la magia
    if (curr_index == PIXEL_SIZE){
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

  return buffer;
}

ByteBuffer *LSB4_retrieve(PixelNode *carrier)
{
  printf("TODO: Implement :)\n");
  return NULL;
}

ByteBuffer *LSBE_retrieve(PixelNode *carrier)
{
  printf("TODO: Implement :)\n");
  return NULL;
}


PixelNode *steg_apply(ByteBuffer *msg, PixelNode *carrier, StegAlgorithm algorithm)
{
  static PixelNode *(*alg_appl_ptr_arr[])(ByteBuffer *, PixelNode *) = {LSB1_apply, LSB4_apply, LSBE_apply};
  return (*alg_appl_ptr_arr[algorithm])(msg, carrier);
}

ByteBuffer *steg_retrieve(PixelNode *carrier, StegAlgorithm algorithm)
{
  static ByteBuffer *(*alg_rtrv_ptr_arr[])(PixelNode *) = {LSB1_retrieve, LSB4_retrieve, LSBE_retrieve};
  return (*alg_rtrv_ptr_arr[algorithm])(carrier);
}