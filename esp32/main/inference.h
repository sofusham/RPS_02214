
#define NUMBER_OF_CLASSES 3

bool inference_allocating_memory();
bool inference_init();
void inference_print_tensor_type(void);
float* inference_put_features(const float *features, uint16_t resize_w, uint16_t resize_h);
bool inference_predict(float *prediction);