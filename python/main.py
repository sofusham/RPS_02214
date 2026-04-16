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
        transforms.Resize((height, width)),
        transforms.ToTensor(),
    ])

    train_dataset = datasets.ImageFolder("python/data/train", transform=transform)
    test_dataset = datasets.ImageFolder("python/data/test", transform=transform)

    train_loader = DataLoader(
        train_dataset,
        batch_size=batch_size,
        shuffle=True,
        num_workers=4
    )

    test_loader = DataLoader(
        test_dataset,
        batch_size=batch_size,
        shuffle=False,
        num_workers=4
    )

    num_classes = 3
    model.classifier[1] = nn.Linear(1280, num_classes)

    for param in model.features.parameters():
        param.requires_grad = False


    criterion = nn.CrossEntropyLoss()
    optimizer = torch.optim.Adam(model.classifier.parameters(), lr=1e-4)

    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model.to(device)

    EPOCHS = 30
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

    # Evaluate the model on test data
    model.eval()
    all_preds = []
    all_labels = []

    with torch.no_grad():
        for images, labels in test_loader:
            images = images.to(device)
            labels = labels.to(device)

            outputs = model(images)
            _, preds = torch.max(outputs, 1)

            all_preds.extend(preds.cpu().numpy())
            all_labels.extend(labels.cpu().numpy())

    # Generate confusion matrix
    cm = confusion_matrix(all_labels, all_preds)
    print("Confusion Matrix:")
    print(cm)

    # Print classification report
    class_names = test_dataset.classes  # ['paper', 'rock', 'scissors']
    print("\nClassification Report:")
    print(classification_report(all_labels, all_preds, target_names=class_names))

    # Visualize confusion matrix
    plt.figure(figsize=(8, 6))
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', 
                xticklabels=class_names, yticklabels=class_names)
    plt.title('Confusion Matrix')
    plt.ylabel('True Label')
    plt.xlabel('Predicted Label')
    plt.tight_layout()
    plt.savefig('confusion_matrix.png')
    plt.show()

    # Calculate and print accuracy and f1-score
    accuracy = (np.array(all_preds) == np.array(all_labels)).mean()
    print(f"\nTest Accuracy: {accuracy:.4f}")
    print(f"\nF1-score: {f1_score(all_labels, all_preds)}")