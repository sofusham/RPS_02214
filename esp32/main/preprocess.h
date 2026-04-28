

#define RESIZE_W 64
#define RESIZE_H 64 
#define MAX_CROP 240*240

void preprocess_pipeline(const uint8_t* input, uint8_t* output,
                         int in_w, int in_h, int out_w, int out_h);