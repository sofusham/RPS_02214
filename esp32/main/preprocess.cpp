
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

            cropped[y * crop_size + x] = input[src_y * in_w + src_x];
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

void preprocess_normalize_image(const uint8_t* input, float* output, int size) {
    for (int i = 0; i < size; i++) {
        // Convert to [0,1]
        float x = input[i] / 255.0f;

        // Normalize: (x - 0.5) / 0.5  → [-1, 1]
        output[i] = (x - 0.5f) / 0.5f;

        // Equivalent faster version:
        // output[i] = (input[i] / 127.5f) - 1.0f;
    }
}




// void preprocess_pipeline(const uint8_t* input, float* output,
//                          int in_w, int in_h, int out_w, int out_h){

//     int crop_size = (in_w < in_h) ? in_w : in_h;
//     static uint8_t temp_buffer1[MAX_CROP];
//     static uint8_t temp_buffer2[MAX_CROP];
        
//     // Step 1: Center crop
//     preprocess_center_crop(input, temp_buffer1, in_w, in_h);
//     // Step 2: Resize
//     preprocess_resize_nearest(temp_buffer1, temp_buffer2, crop_size, crop_size, out_w, out_h);
//     // Step 3: Since the model is not quantized we will normalize the image. 
//     preprocess_normalize_image(temp_buffer2, output, out_w * out_h);
// }

void preprocess_crop_resize_normalize(const uint8_t* input, float* output,
                                      int in_w, int in_h, int out_w, int out_h){
    // Determine square crop
    int crop_size = (in_w < in_h) ? in_w : in_h;
    int x_offset = (in_w - crop_size) / 2;
    int y_offset = (in_h - crop_size) / 2;

    for (int y = 0; y < out_h; y++) {
        for (int x = 0; x < out_w; x++) {

            // Map output pixel → cropped input
            int src_x = x_offset + (x * crop_size) / out_w;
            int src_y = y_offset + (y * crop_size) / out_h;

            uint8_t pixel = input[src_y * in_w + src_x];

            // Normalize directly
            output[y * out_w + x] = (pixel / 127.5f) - 1.0f;
        }
    }
}
