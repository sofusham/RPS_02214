
// /* 
//     OBS, part of this code was copy-pasted directly from "DTU-02214/keywords/esp32/main/inference.cpp".
// */


// Include TFLM
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"

// #include "inference.h"

#include "model.h"
#include "serial.h"

// Tensor arena size, found by trial and error
#define TENSOR_ARENA_SIZE (30 * 1024)

// Static variables
static const tflite::Model *model = nullptr;
static tflite::MicroInterpreter *interpreter = nullptr;
alignas(16) static uint8_t tensor_arena[TENSOR_ARENA_SIZE];
static TfLiteTensor *input = nullptr;
static TfLiteTensor *output = nullptr;
static const char *TAG_INF = "Inference";


bool inference_init(){
    // Load TFlite model
    model = tflite::GetModel(model_binary);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        return false;
    }

        // Create an interpreter
    static tflite::MicroMutableOpResolver<9> micro_op_resolver;
    // Conv1D
    micro_op_resolver.AddReshape();
    micro_op_resolver.AddConv2D();
    // MaxPool1D
    micro_op_resolver.AddMaxPool2D();
    // Flatten
    micro_op_resolver.AddShape();
    micro_op_resolver.AddExpandDims();
    micro_op_resolver.AddStridedSlice();
    micro_op_resolver.AddPack();
    // Dense with sigmoid activation
    micro_op_resolver.AddFullyConnected();
    micro_op_resolver.AddSoftmax();
    static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver, tensor_arena, TENSOR_ARENA_SIZE);
    interpreter = &static_interpreter;

    // Allocate memory for input and output tensors
    if (interpreter->AllocateTensors() != kTfLiteOk)
    {
        return false;
    }

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


