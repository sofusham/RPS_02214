
// /* 
//     OBS, part of this code was copy-pasted directly from "DTU-02214/keywords/esp32/main/inference.cpp".
// */


// Include TFLM
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"

#include "inference.h"

#include "model.h"
#include "serial.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Tensor arena size, found by trial and error
#define TENSOR_ARENA_SIZE (300 * 1024)

// Static variables
static const tflite::Model *model = nullptr;
static tflite::MicroInterpreter *interpreter = nullptr;
// alignas(16) static uint8_t tensor_arena[TENSOR_ARENA_SIZE];
// Replace with a pointer:
uint8_t* tensor_arena = nullptr;

static TfLiteTensor *input = nullptr;
static TfLiteTensor *output = nullptr;
static const char *TAG_INF = "Inference";


static void softmax(float* input, int length) {
    float max_val = input[0];
    for (int i = 1; i < length; i++) {
        if (input[i] > max_val) max_val = input[i];
    }
    float sum = 0.0f;
    for (int i = 0; i < length; i++) {
        input[i] = expf(input[i] - max_val);
        sum += input[i];
    }
    for (int i = 0; i < length; i++) {
        input[i] /= sum;
    }
}



bool inference_allocating_memory(){
    tensor_arena = (uint8_t*)heap_caps_malloc(TENSOR_ARENA_SIZE, MALLOC_CAP_SPIRAM);   
    if (tensor_arena == nullptr) {
        serial_printf("Failed to allocate tensor arena in PSRAM!\n");
        return false;
    }   
    return true;
}

bool inference_init(){
    // Load TFlite model
    model = tflite::GetModel(model_binary);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        // ESP_LOGE(TAG_INF, "Model schema mismatch!");
        printf("Model schema mismatch!\n");
        return false;
    }


    // Create an interpreter
    static tflite::MicroMutableOpResolver<13> micro_op_resolver; // bump to 12
    
    // //Original model. 
    // // Conv1D
    // micro_op_resolver.AddReshape();
    // micro_op_resolver.AddConv2D();
    // // MaxPool1D
    // micro_op_resolver.AddMaxPool2D();
    // // Flatten
    // micro_op_resolver.AddShape();
    // micro_op_resolver.AddExpandDims();
    // micro_op_resolver.AddStridedSlice();
    // micro_op_resolver.AddPack();
    // // Dense with sigmoid activation
    // micro_op_resolver.AddFullyConnected();
    // micro_op_resolver.AddSoftmax();

    //CCN model attempt 
    // micro_op_resolver.AddConv2D();
    // micro_op_resolver.AddDepthwiseConv2D();
    // micro_op_resolver.AddAveragePool2D();
    // micro_op_resolver.AddFullyConnected();
    // micro_op_resolver.AddReshape();
    // micro_op_resolver.AddSoftmax();
    // micro_op_resolver.AddAdd();
    // micro_op_resolver.AddMul();
    // micro_op_resolver.AddRelu();

    // If your TFLM build supports it:
    // micro_op_resolver.AddLeakyRelu();

    micro_op_resolver.AddConv2D();
    micro_op_resolver.AddDepthwiseConv2D(); 
    micro_op_resolver.AddAveragePool2D();
    micro_op_resolver.AddFullyConnected();
    micro_op_resolver.AddReshape();
    micro_op_resolver.AddSoftmax();
    micro_op_resolver.AddAdd();
    micro_op_resolver.AddMul();
    micro_op_resolver.AddRelu();
    micro_op_resolver.AddLeakyRelu();
    micro_op_resolver.AddPadV2();
    micro_op_resolver.AddPad();
    micro_op_resolver.AddSum();  // <-- add this

    static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver, tensor_arena, TENSOR_ARENA_SIZE);
    interpreter = &static_interpreter;

    // printf("Before calling the static interpreter\n");
    // vTaskDelay(pdMS_TO_TICKS(3000));
    // Allocate memory for input and output tensors
    if (interpreter->AllocateTensors() != kTfLiteOk)
    {
        return false;
    }
    // printf("After calling the static interpreter\n");
    // vTaskDelay(pdMS_TO_TICKS(3000));

    // Get pointers for input and output tensors
    input = interpreter->input(0);
    output = interpreter->output(0);

    // Print input and output tensor types and dimensions
    serial_printf("Input tensor type: %s, shape: %d, %d, %d\n",
             TfLiteTypeGetName(input->type), input->dims->data[0], input->dims->data[1], input->dims->data[2]);
    serial_printf("Output tensor type: %s, shape: %d, %d\n",
             TfLiteTypeGetName(output->type), output->dims->data[0], output->dims->data[1]);
    return true; 

    // ESP_LOGI(TAG_INF, "Input tensor type: %s, shape: %d, %d, %d",
    //          TfLiteTypeGetName(input->type), input->dims->data[0], input->dims->data[1], input->dims->data[2]);
    // ESP_LOGI(TAG_INF, "Output tensor type: %s, shape: %d, %d",
    //          TfLiteTypeGetName(output->type), output->dims->data[0], output->dims->data[1]);
    // return true;

}


void inference_print_tensor_type(void){
    serial_printf("Input tensor type: %d\n", input->type);
}

float* inference_put_features(const float *features, uint16_t resize_w, uint16_t resize_h){
    for(uint16_t i = 0; i < resize_w * resize_h; ++i){
        input->data.f[i] = features[i];
    }
    return input->data.f;
}


bool inference_predict(float *prediction){
    if(interpreter->Invoke() != kTfLiteOk){
        return false;
    }
    // First copy raw logits
    for(uint8_t i = 0; i < NUMBER_OF_CLASSES; ++i){
        prediction[i] = output->data.f[i];
    }
    // Apply softmax to convert logits to probabilities
    softmax(prediction, NUMBER_OF_CLASSES);
    return true;
}