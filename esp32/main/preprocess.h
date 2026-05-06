

#define RESIZE_W 64
#define RESIZE_H 64 
#define MAX_CROP 240*240

// void preprocess_pipeline(const uint8_t* input, float* output,
//                          int in_w, int in_h, int out_w, int out_h);

void preprocess_crop_resize_normalize(const uint8_t* input, float* output,
            int in_w, int in_h, int out_w, int out_h);