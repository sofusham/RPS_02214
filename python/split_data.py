import os
import random
import shutil

# Paths
base_dir = "python/data/mandens"
classes = ["rock", "paper", "scissors"]

train_ratio = 0.8  # 80% train, 20% test

for cls in classes:
    src = os.path.join(base_dir, cls)
    
    images = os.listdir(src)
    random.shuffle(images)
    
    split_idx = int(len(images) * train_ratio)
    
    train_files = images[:split_idx]
    test_files = images[split_idx:]
    
    # Create directories
    for split in ["train", "test"]:
        os.makedirs(os.path.join(base_dir, split, cls), exist_ok=True)
    
    # Move files
    for f in train_files:
        shutil.copy(os.path.join(src, f),
                    os.path.join(base_dir, "train", cls, f))
    
    for f in test_files:
        shutil.copy(os.path.join(src, f),
                    os.path.join(base_dir, "test", cls, f))

print("Done splitting dataset!")