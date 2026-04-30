"""
Reuqires python 3.13
And packages from requirements.txt (pip install -r requirements.txt)
Might interfere with previous intalled packages
"""

import torch
import torchvision
import litert_torch
import os

# Function from https://github.com/gustaf-hammarberg/DTU-02214
def write_model_c_file(path: str, tflite_model):
    # Ensure that the folder exists
    os.makedirs(os.path.dirname(path), exist_ok=True)

    # Write source file
    with open(path, "w") as c_file:
        c_file.write("const unsigned char model_binary[] = {\n")
        for i, byte in enumerate(tflite_model):
            c_file.write(f"0x{byte:02x}, ")
            if (i + 1) % 12 == 0:
                c_file.write("\n")
        c_file.write("\n};\n")

# Function from https://github.com/gustaf-hammarberg/DTU-02214
def write_model_h_file(path: str, defines: dict, declarations: list[str]):
    # Ensure that the folder exists
    os.makedirs(os.path.dirname(path), exist_ok=True)

    # Write header file
    with open(path, "w") as h_file:
        h_file.write("#ifndef MODEL_H\n")
        h_file.write("#define MODEL_H\n")
        h_file.write("\n")
        for key, value in defines.items():
            h_file.write(f'#define {key} {value}\n')
        h_file.write("\n")
        for declaration in declarations:
            h_file.write(f'{declaration}\n')
        h_file.write("\n")
        h_file.write("extern const unsigned char model_binary[];\n")
        h_file.write("\n")
        h_file.write("#endif\n")

def convert_pytorch_to_c(model, sample_inputs, output_dir="esp32/main/"):
    """
    Sample inputs should be a tuple of tensors, e.g. (torch.randn(1, 3, 64, 64),)
    For grayscale images, use sample_inputs = (torch.randn(1, 1, height, width),)
    """
    # Convert model
    edge_model = litert_torch.convert(model.eval(), sample_inputs)
    edge_model.export("temp.tflite")

    # Read tflite file
    with open("temp.tflite", "rb") as f:
        tflite_model = f.read()

    # write
    write_model_c_file(f"{output_dir}model.c",
                       tflite_model)
    
    write_model_h_file(f"{output_dir}model.h",
                       defines={"MODEL_LEN": len(tflite_model)},
                       declarations=[])

    print(f"Model converted and saved to {output_dir}")