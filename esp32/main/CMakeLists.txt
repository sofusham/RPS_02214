idf_component_register(SRCS "main.cpp" "camera.cpp" "serial.cpp" "preprocess.cpp" "util.cpp" "inference.cpp" "model.c"
                    INCLUDE_DIRS "."
                    REQUIRES espressif__esp-tflite-micro nvs_flash
                    PRIV_REQUIRES esp_timer)


