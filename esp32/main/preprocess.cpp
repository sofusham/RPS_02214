
#include <cstdarg>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "driver/usb_serial_jtag.h"



#include "serial.h"
#include "preprocess.h"


void preprocess_resize_nearest(const uint8_t* input, uint8_t* output,
                    int in_w, int in_h, int out_w, int out_h) {

    for(int i = 0; i < out_h; i++) {
        int x = i * in_h / out_h;
        for(int j = 0; j < out_w; j++) {
            int y = j * in_w / out_w;
            output[i * out_w + j] = input[x * in_w + y];
        }
    }
}
