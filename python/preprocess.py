
# Training: 
#Two different size of images will be used: 
#   - images taken with the esp32: width: 320, height: 240
#   - images taken from a dataset: width: 300, height: 300 

#Inference: 
#Only the images taken with the esp32: width: 320, height: 240.

#the pipeling: 
#     grayscale -> center_crop -> resize

import numpy as np
import cv2
import re

ESP_IMG_WIDTH = 320
ESP_IMG_HEIGHT = 240

RESIZE_WIDTH = 64
RESIZE_HEIGHT = 64

in_dir = 'data/'

def preprocess_grey(img): 
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    return gray

def preprocess_center_crop(img, in_w, in_h): 
    crop_size = min(in_w, in_h)
    cropped = np.zeros((crop_size, crop_size), dtype=np.uint8)
    
    x_offset = (in_w - crop_size) // 2
    y_offset = (in_h - crop_size) // 2

    for y in range(crop_size):
        for x in range(crop_size):
            cropped[y, x] = img[y + y_offset, x + x_offset]
    
    return  cropped, crop_size 

def preprocess_resize(img, in_w, in_h, out_w, out_h):
    resized = np.zeros((out_h, out_w), dtype=np.uint8)
    for i in range(out_h):
        x = int(i * in_h // out_h)
        for j in range(out_w):
            y = int(j * in_w // out_w)
            resized[i, j] = img[x, y]
    return resized

def preprocess_pipeline_esp_img(in_path, out_path, in_w, in_h, out_w, out_h):
    img = cv2.imread(in_path)
    #step 1: grayscale
    grey = preprocess_grey(img)
    #step 2: center crop 
    cropped, crop_size = preprocess_center_crop(grey, in_w, in_h) 
    #step 3: resize
    resized = preprocess_resize(cropped, crop_size, crop_size, out_w, out_h)
    cv2.imwrite(out_path, resized.astype(np.uint8))
    
    
def preprocess_pipeline_dataset_img(in_path, out_path, in_w, in_h, out_w, out_h):
    img = cv2.imread(in_path)
    #step 1: grayscale
    grey = preprocess_grey(img)
    #step 2: resize
    grey_height, grey_width = grey.shape
    resized = preprocess_resize(grey, grey_height, grey_width, out_w, out_h)
    cv2.imwrite(out_path, resized.astype(np.uint8))


# preprocess_pipeline_esp_img(in_dir + '/5/' + 'image_20260428_162839.png', in_dir + '/5/' + 'esp_img_preprocessed.png', 
#                             ESP_IMG_WIDTH, ESP_IMG_HEIGHT, RESIZE_WIDTH, RESIZE_HEIGHT)


# preprocess_pipeline_dataset_img(in_dir + 'paper_1.png', in_dir + 'dataset_img_preprocessed.png',
#                             ESP_IMG_WIDTH, ESP_IMG_HEIGHT, RESIZE_WIDTH, RESIZE_HEIGHT)


img1 = cv2.imread(in_dir + '/5/' + "image_20260428_162502.png", cv2.IMREAD_GRAYSCALE)
img2 = cv2.imread(in_dir + '/5/' + "esp_img_preprocessed.png", cv2.IMREAD_GRAYSCALE)

diff = cv2.absdiff(img1, img2)

print("Max diff:", np.max(diff))
print("Any difference:", np.any(diff))

# img2 = cv2.imread(in_dir + '/5/' + "esp_img_preprocessed.png")
img3 = cv2.imread(in_dir + 'dataset_img_preprocessed.png', cv2.IMREAD_GRAYSCALE)

print(f"esp_img_preprocessed shape: {img2.shape}")
print(f"esp_img_preprocessed type: {img2.dtype}")
print(f"dataset_img_preprocessed shape: {img3.shape}")
print(f"dataset_img_preprocessed type: {img3.dtype}")