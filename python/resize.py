import numpy as np
import cv2

in_dir = 'python/data/'

def resize_gray_img(in_path, out_path):
    img = cv2.imread(in_path)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    resized = cv2.resize(gray, (64, 64), interpolation=cv2.INTER_AREA)
    cv2.imwrite(out_path, resized)

resize_gray_img(in_dir + 'paper_1.png', in_dir + 'paper_1_resized.png')