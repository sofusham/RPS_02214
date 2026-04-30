import torch
import torch.nn as nn
import torch.nn.functional as F
from torchvision import transforms, datasets
from torch.utils.data import DataLoader, ConcatDataset
from utils.model_converter import convert_pytorch_to_c

class TinyRPSNet(nn.Module):
    def __init__(self, num_classes=3):
        super().__init__()

        # Initial conv
        self.conv1 = nn.Conv2d(
            in_channels=1,
            out_channels=8,
            kernel_size=3,
            stride=2,
            padding=1
        )
        self.bn1 = nn.BatchNorm2d(8)

        # Depthwise + Pointwise block 1
        self.dw1 = nn.Conv2d(
            in_channels=8,
            out_channels=8,
            kernel_size=3,
            padding=1,
            groups=8  # depthwise
        )
        self.bn_dw1 = nn.BatchNorm2d(8)
        self.pw1 = nn.Conv2d(
            in_channels=8,
            out_channels=16,
            kernel_size=1
        )
        self.bn_pw1 = nn.BatchNorm2d(16)
        # Depthwise + Pointwise block 2
        self.dw2 = nn.Conv2d(
            in_channels=16,
            out_channels=16,
            kernel_size=3,
            padding=1,
            groups=16
        )
        self.bn_dw2 = nn.BatchNorm2d(16)
        self.pw2 = nn.Conv2d(
            in_channels=16,
            out_channels=32,
            kernel_size=1
        )
        self.bn_pw2 = nn.BatchNorm2d(32)

        # Global average pooling + classifier
        self.pool = nn.AdaptiveAvgPool2d(1)
        self.fc = nn.Linear(32, num_classes)

    def forward(self, x):
        # x: (B, 1, 64, 64)
        self.act = nn.LeakyReLU(0.1)

        x = self.act(self.bn1(self.conv1(x)))
        x = self.act(self.bn_pw1(self.pw1(self.bn_dw1(self.dw1(x)))))
        x = self.act(self.bn_pw2(self.pw2(self.bn_dw2(self.dw2(x)))))

        x = self.pool(x)           # (B, 32, 1, 1)
        x = torch.flatten(x, 1)    # (B, 32)
        x = self.fc(x)             # (B, num_classes)

        return x  # logits (no softmax here)
    
device = "cpu"
model = TinyRPSNet(num_classes=3).to(device)
model.load_state_dict(torch.load("python/model.pth"))
model.eval()

sample_inputs = (torch.randn(1, 1, 64, 64),)
convert_pytorch_to_c(model, sample_inputs)