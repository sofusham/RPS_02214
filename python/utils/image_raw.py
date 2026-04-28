import cv2
import numpy as np

in_dir = '../data/ESP_test_img/'


img = cv2.imread(in_dir + "esp_test_img.png")
# gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

# Make sure it's 320x240
gray = cv2.resize(img, (320, 240))

# Save raw bytes
gray.tofile("image.raw")

data = np.fromfile("image.raw", dtype=np.uint8)

with open("image.h", "w") as f:
    f.write("const uint8_t test_image[] = {\n")
    for i, val in enumerate(data):
        f.write(f"{val},")
        if i % 20 == 0:
            f.write("\n")
    f.write("};\n")
    
    