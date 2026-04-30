if __name__ == "__main__":
    import torch
    import torch.nn as nn
    from torchvision import models, transforms, datasets
    from torch.utils.data import DataLoader

    from sklearn.metrics import confusion_matrix, classification_report, roc_auc_score, f1_score
    import matplotlib.pyplot as plt
    import seaborn as sns
    import numpy as np

    model = models.mobilenet_v2(pretrained=True)

    height = 64
    width = 64
    batch_size = 32
    
    transform = transforms.Compose([
        transforms.Grayscale(num_output_channels=1),
        transforms.Resize((height, width)),
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.5], std=[0.5])
    ])

    train_dataset = datasets.ImageFolder("python/data/train", transform=transform)
    train_val_dataset = datasets.ImageFolder("python/data/train_val", transform=transform)
    validation_dataset = datasets.ImageFolder("python/data/validation", transform=transform)

    train_loader = DataLoader(
        train_dataset,
        batch_size=batch_size,
        shuffle=True,
        num_workers=4
    )

    train_val_loader = DataLoader(
        train_val_dataset,
        batch_size=batch_size,
        shuffle=False,
        num_workers=4
    )

    validation_loader = DataLoader(
        validation_dataset,
        batch_size=batch_size,
        shuffle=False,
        num_workers=4
    )

    num_classes = 3
    
    # Adapt first conv layer to accept grayscale (1 channel) instead of RGB (3 channels)
    model.features[0][0] = nn.Conv2d(1, 32, kernel_size=3, stride=2, padding=1, bias=False)
    
    model.classifier[1] = nn.Linear(1280, num_classes)

    for param in model.features.parameters():
        param.requires_grad = False


    criterion = nn.CrossEntropyLoss()
    optimizer = torch.optim.Adam(model.classifier.parameters(), lr=1e-4)

    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model.to(device)

    EPOCHS = 30
    print(f"\nStarting training for {EPOCHS} epochs...")
    for epoch in range(EPOCHS):
        model.train()

        for images, labels in train_loader:
            images = images.to(device)
            labels = labels.to(device)

            outputs = model(images)
            loss = criterion(outputs, labels)

            optimizer.zero_grad()
            loss.backward()
            optimizer.step()

        print(f"Epoch [{epoch+1}/{EPOCHS}], Loss: {loss.item():.4f}")


    def evaluate_model(model, data_loader):
        model.eval()
        all_preds = []
        all_labels = []

        with torch.no_grad():
            for images, labels in data_loader:
                images = images.to(device)
                labels = labels.to(device)

                outputs = model(images)
                _, preds = torch.max(outputs, 1)

                all_preds.extend(preds.cpu().numpy())
                all_labels.extend(labels.cpu().numpy())

        return all_labels, all_preds

    print("\nEvaluating on train validation set...")
    all_labels, all_preds = evaluate_model(model, train_val_loader)
    # Generate confusion matrix
    cm = confusion_matrix(all_labels, all_preds)
    print("\nConfusion Matrix (Train Validation):")
    print(cm)

    # Print classification report
    class_names = train_val_dataset.classes  # ['paper', 'rock', 'scissors']
    print("\nClassification Report (Train Validation):")
    print(classification_report(all_labels, all_preds, target_names=class_names))


    print("\nEvaluating on validation set...")
    all_labels_val, all_preds_val = evaluate_model(model, validation_loader)
    # Generate confusion matrix
    cm_val = confusion_matrix(all_labels_val, all_preds_val)
    print("\nConfusion Matrix (Validation):")
    print(cm_val)

    # Print classification report
    class_names = validation_dataset.classes  # ['paper', 'rock', 'scissors']
    print("\nClassification Report (Validation):")
    print(classification_report(all_labels_val, all_preds_val, target_names=class_names))



"""     # Visualize confusion matrix
    plt.figure(figsize=(8, 6))
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', 
                xticklabels=class_names, yticklabels=class_names)
    plt.title('Confusion Matrix')
    plt.ylabel('True Label')
    plt.xlabel('Predicted Label')
    plt.tight_layout()
    plt.savefig('confusion_matrix.png')
    plt.show() """