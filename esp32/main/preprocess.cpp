
#include <cstdarg>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "driver/usb_serial_jtag.h"



#include "serial.h"
#include "preprocess.h"

void preprocess_center_crop(const uint8_t* input, uint8_t* cropped, int in_w, int in_h) {
    int crop_size = (in_w < in_h) ? in_w : in_h;

    int x_offset = (in_w - crop_size) / 2;
    int y_offset = (in_h - crop_size) / 2;

    for (int y = 0; y < crop_size; y++) {
        for (int x = 0; x < crop_size; x++) {

            int src_x = x + x_offset;
            int src_y = y + y_offset;

            cropped[y * crop_size + x] =
                input[src_y * in_w + src_x];
        }
    }
}

void preprocess_resize_nearest(const uint8_t* input, uint8_t* resized,
                               int in_w, int in_h, int out_w, int out_h) {

    for(int i = 0; i < out_h; i++) {
        int x = i * in_h / out_h;
        for(int j = 0; j < out_w; j++) {
            int y = j * in_w / out_w;
            resized[i * out_w + j] = input[x * in_w + y];
        }
    }
}


void preprocess_pipeline(const uint8_t* input, uint8_t* output,
                         int in_w, int in_h, int out_w, int out_h){

    int crop_size = (in_w < in_h) ? in_w : in_h;
    static uint8_t temp_buffer[MAX_CROP];
                            
    // Step 1: Center crop
    preprocess_center_crop(input, temp_buffer, in_w, in_h);
    // Step 2: Resize
    preprocess_resize_nearest(temp_buffer, output, crop_size, crop_size, out_w, out_h);
    // Since the model is quantized it will expect uint8_t. No further steps needed. 
}