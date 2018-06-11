#include "api.h"

PixelNode *LSB1_apply(ByteBuffer *msg, PixelNode *carrier)
{
  printf("TODO: Implement :)\n");
  return NULL;
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