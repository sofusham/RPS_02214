
# Training: 
#Two different size of images will be used: 
#   - images taken with the esp32: width: 320, height: 240
#   - images taken from a dataset: width: 300, height: 300 

#Inference: 
#Only the images taken with the esp32: width: 320, height: 240.







import numpy as np
import cv2
import re








in_dir = 'data/'

#both resize_gray_img and resize_gray_manual returns the pixel value 186 at (30,30)
#it is 1 higher than on the esp, but should not make a big difference.

# def resize_gray_img(in_path, out_path):
#     img = cv2.imread(in_path)
#     gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
#     resized = cv2.resize(gray, (64, 64), interpolation=cv2.INTER_NEAREST)
#     cv2.imwrite(out_path, resized)

def resize_gray_manual(in_path, out_path, in_w, in_h, out_w, out_h):
    img = cv2.imread(in_path)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    resized = np.zeros((out_h, out_w), dtype=np.uint8)
    for i in range(out_h):
        x = int(i * in_h // out_h)
        for j in range(out_w):
            y = int(j * in_w // out_w)
            resized[i, j] = gray[x, y]
    cv2.imwrite(out_path, resized)
  
  
# void preprocess_resize_nearest(const uint8_t* input, uint8_t* output,
#                     int in_w, int in_h, int out_w, int out_h) {

#     for(int i = 0; i < out_h; i++) {
#         int x = i * in_h / out_h;
#         for(int j = 0; j < out_w; j++) {
#             int y = j * in_w / out_w;
#             output[i * out_w + j] = input[x * in_w + y];
#         }
#     }
# }
  
  
  
  
  #this was used to compare the output when using exactly the same image as on the esp. 
  #it returned the value 185 at (30,30) in the resized image, which matches the value printed by the esp32 after preprocessing
    
# def load_c_array(path, width, height):
#     with open(path, "r") as f:
#         text = f.read()

#     # extract content inside braces
#     data = re.search(r'\{([^}]*)\}', text).group(1)

#     numbers = list(map(int, re.findall(r'\d+', data)))

#     arr = np.array(numbers, dtype=np.uint8)
#     return arr.reshape((height, width))


# def resize_gray_manual_from_array(input_array, out_path):
#     in_h, in_w = input_array.shape

#     resized = np.zeros((64, 64), dtype=np.uint8)

#     for i in range(64):
#         for j in range(64):
#             x = (i * in_h) // 64
#             y = (j * in_w) // 64
#             resized[i, j] = input_array[x, y]
#     cv2.imwrite(out_path, resized)

img = cv2.imread(in_dir + 'paper_1.png')
print(f"Original image shape: {img.shape}")


# resize_gray_img(in_dir + 'paper_1.png', in_dir + 'paper_1_resized2.png')
resize_gray_manual(in_dir + 'paper_1.png', in_dir + 'paper_1_resized_manual.png', 240, 320, 64, 64)
# test_image = load_c_array('utils/'+ 'image.h', 320, 240)
# resize_gray_manual_from_array(test_image, in_dir + 'test_image_resized.png')

# print("pixel value at (30,30) in resized image:", cv2.imread(in_dir + 'paper_1_resized2.png', cv2.IMREAD_GRAYSCALE)[30, 30])
print("pixel value at (30,30) in manually resized image:", cv2.imread(in_dir + 'paper_1_resized_manual.png', cv2.IMREAD_GRAYSCALE)[30, 30])
# print("pixel value at (30,30) in C array resized image:", cv2.imread(in_dir + 'test_image_resized.png', cv2.IMREAD_GRAYSCALE)[30, 30])



