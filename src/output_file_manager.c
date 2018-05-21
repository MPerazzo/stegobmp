#include "api.h"
#include <string.h>

void overwrite_pixel_list(PixelNode *list, InputFile *input_file, Steg steg_algorithm)
{

    int pixel_index = 0;
    PixelNode *pixel_node = list;    
    Pixel *pixel = &pixel_node->pixel;
    u_int8_t *component[] = {&pixel->blue, &pixel->green, &pixel->red};

    if (steg_algorithm == LSB1)
    {
        for (u_int8_t i = 0; i < input_file->size; i++)
        {
            u_int8_t byte;
            memcpy(&byte, input_file->file->start + i, 1);
            for (int j = 7; j >= 0; j--)
            {
                /* Get bit from input file */
                u_int8_t bit = (byte >> j) & 1;
                
                /* Hide bit in LSB1 */
                u_int8_t new_component = (*component[pixel_index] & 0xFE) | bit;
                memcpy(component[pixel_index], &new_component, 1);
                
                pixel_index++;

                if (pixel_index == 3)
                {
                    pixel_node = pixel_node->next; /* Next pixel */
                    pixel = &pixel_node->pixel;
                    pixel_index = 0;
                    component[0] = &pixel->blue;
                    component[1] = &pixel->green;
                    component[2] = &pixel->red;
                }
            }
        }
    }    
}

void *save_output_file(PixelNode *list, char *filename, ByteBuffer *carrier_header)
{
    FILE *fd;
    if ((fd = fopen(filename, "wb")) == NULL)
    {
        return NULL;
    }
    
    /*TODO: invert pixel list and add padding */

    fclose(fd);
    return (void *)1;
}