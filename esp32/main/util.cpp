
#include <stdint.h>
#include <stdio.h>

#include "util.h"

void util_find_min_max_uint8_t(uint8_t *image, int size, uint8_t *min_val, uint8_t *max_val) {
    *min_val = 255.0f;
    *max_val = 0.0f;

    for (int i = 0; i < size; i++) {
        if (image[i] < *min_val) {
            *min_val = image[i];
        }
        if (image[i] > *max_val) {
            *max_val = image[i];
        }
    }
}

void util_find_min_max_float(float *image, int size, float *min_val, float *max_val) {
    *min_val = __FLT_MAX__;
    *max_val = __FLT_MIN__;

    for (int i = 0; i < size; i++) {
        if (image[i] < *min_val) {
            *min_val = image[i];
        }
        if (image[i] > *max_val) {
            *max_val = image[i];
        }
    }
}