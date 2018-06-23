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
  printf("TODO: Implement :)\n");
  return NULL;
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