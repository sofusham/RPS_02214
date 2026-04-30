
#include <stdint.h>
#include <stdio.h>

#include "util.h"

void util_find_min_max(uint8_t *image, int size, uint8_t *min_val, uint8_t *max_val) {
    *min_val = 255;
    *max_val = 0;

    for (int i = 0; i < size; i++) {
        if (image[i] < *min_val) {
            *min_val = image[i];
        }
        if (image[i] > *max_val) {
            *max_val = image[i];
        }
    }
}