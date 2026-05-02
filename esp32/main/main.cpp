#include <cstdio>
#include <cstdint>
#include <cstring>

// ESP includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "driver/usb_serial_jtag.h"
#include "esp_task_wdt.h"
#include <esp_timer.h>

// Project includes
#include "camera.h"
#include "serial.h"
#include "preprocess.h"
#include "util.h"
#include "inference.h"
#include "model.h"

// Static constants and variables
static constexpr size_t CHUNK_SIZE = 256;
static const char* FRAME_PREAMBLE = "\n===FRAME===\n";
static uint8_t image_buffer[FRAME_W * FRAME_H * FRAME_C];
static float image_resized[RESIZE_W * RESIZE_H * FRAME_C];
static int number_of_pixels_image_resized = RESIZE_W * RESIZE_H * FRAME_C;

static float prediction[NUMBER_OF_CLASSES];

void setup()
{
    // Initialize NVS (required by some drivers)
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Initialize camera
    if (!camera_init()) {
        abort();
    }

    // Initialize USB serial
    usb_serial_jtag_driver_config_t cfg = {
        .tx_buffer_size = 512,
        .rx_buffer_size = 512,
    };
    err = usb_serial_jtag_driver_install(&cfg);
    ESP_ERROR_CHECK(err);

    // Allocate memory for tensor arena before initializing interpreter. 
    if(!inference_allocating_memory()){
        serial_printf("Inference memory allocation failed\n");
        // vTaskDelay(pdMS_TO_TICKS(3000));
    }

    // Initialize inference 
    if(!inference_init()){
        serial_printf("Inference initi failed\n");
        // vTaskDelay(pdMS_TO_TICKS(3000));
        abort();
    }
    

    // Wait for incoming S on serial port
    printf("Send 'S' to start.\n");
    char c;
    do {
        int r = usb_serial_jtag_read_bytes(&c, 1, portMAX_DELAY);
        if (r < 0) {
            abort();
        }
    } while (c != 'S');
}

void loop(void)
{
    //Image is captured in greyscale. 
    camera_capture_frame(image_buffer);

    // Send preamble
    usb_serial_jtag_write_bytes(FRAME_PREAMBLE, strlen(FRAME_PREAMBLE), pdMS_TO_TICKS(1000));

    // Send image over USB console
    // Note that usb_serial_jtag_write_bytes() may fail if writing too many bytes at once, so it's necessary to
    // send in chunks.
    size_t frame_size = sizeof(image_buffer);
    for (size_t offset = 0; offset < frame_size;) {
        size_t to_write = offset + CHUNK_SIZE < frame_size ? CHUNK_SIZE : frame_size - offset;
        int written = usb_serial_jtag_write_bytes(image_buffer + offset, to_write, pdMS_TO_TICKS(1000));
        if (written < to_write) {
            vTaskDelay(1);
        }
        if (written > 0) {
            offset += written;
        }
    }

    //before preprocessing: 
    // serial_printf("Image before preprocessing: \n");
    // serial_printf("Width: %d\n", FRAME_W);
    // serial_printf("Height: %d\n", FRAME_H);
    // serial_printf("Number of bytes to hold the pixel values (single byte = grayscale): %d\n", FRAME_C);

    // float min_val, max_val;
    // util_find_min_max(image_buffer, sizeof(image_buffer), &min_val, &max_val);
    // serial_printf("Min pixel values: %d\n", min_val);
    // serial_printf("Max pixel values: %d\n", max_val);
    //Before preprocessing, the images are in greyscale, and with the dimensions of 320x240. 
    
    //After preprocessing the images should be greyscale and with the dimensions of 64x64. 
    preprocess_crop_resize_normalize(image_buffer, image_resized, FRAME_W, FRAME_H, RESIZE_W, RESIZE_H);
    
    
    //After preprocessing 
    // serial_printf("Image after preprocessing: \n");
    // serial_printf("Width: %d\n", RESIZE_W);
    // serial_printf("Height: %d\n", RESIZE_H);
    // serial_printf("Number of bytes to hold the pixel values (single byte = grayscale): %d\n", FRAME_C);

    // util_find_min_max_float(image_resized, number_of_pixels_image_resized, &min_val, &max_val);
    // serial_printf("Min pixel values: %f\n", min_val);
    // serial_printf("Max pixel values: %f\n", max_val);

    //Checking the input tensor type: 
    // inference_print_tensor_type();

    // Put features into the interpreter's input tensor
    inference_put_features(image_resized, RESIZE_W, RESIZE_H);
    // serial_printf("Starting inference...\n");
    // int64_t start = esp_timer_get_time();

    if(!inference_predict(prediction)){
        serial_printf("Inference failed\n");
    }else{
        // int64_t end = esp_timer_get_time();
        // serial_printf("Inference done: %lld ms\n", (end - start) / 1000);
        // vTaskDelay(pdMS_TO_TICKS(5000));

        // serial_printf("Inference successful. Prediction: \n");
        serial_printf("Rock: %f\t Paper: %f\t Scissors: %f\n", prediction[0], prediction[1], prediction[2]);

        // for(int i = 0; i < NUMBER_OF_CLASSES; ++i){
        //     serial_printf("Class %d: %f\n", i, prediction[i]);
        // }
        vTaskDelay(pdMS_TO_TICKS(1000));

    }

    // serial_printf("Hello from esp\n");

    // Wait ~1 second
    vTaskDelay(pdMS_TO_TICKS(0.01));
}

// ---------- ESP-IDF entry point ----------

extern "C" void app_main()
{
    setup();
    while (true) {
        loop();
    }
}
