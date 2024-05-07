import pandas as pd
from datasets import Dataset, load_metric
from transformers import AutoTokenizer, AutoModelForSequenceClassification
import numpy as np
import torch
import seaborn as sns
import matplotlib.pyplot as plt

tokenizer = AutoTokenizer.from_pretrained("Hello-SimpleAI/chatgpt-detector-roberta")
model = AutoModelForSequenceClassification.from_pretrained("Hello-SimpleAI/chatgpt-detector-roberta")

def load_and_tokenize_dataset(dataset_path):
    test_data = pd.read_csv(dataset_path)
    test_dataset = Dataset.from_pandas(test_data)
    def tokenize_function(examples):
        return tokenizer(examples["text"], padding="max_length", truncation=True)
    tokenized_dataset = test_dataset.map(tokenize_function, batched=True)
    return tokenized_dataset

def is_ai_generated(file_path, model, tokenizer):
    # Read the content of the file
    with open(file_path, 'r') as file:
        text = file.read().strip()  # Read the file content and strip any leading/trailing whitespace

    inputs = tokenizer(text, return_tensors="pt", max_length=512, truncation=True)

    with torch.no_grad():
        outputs = model(**inputs)

    predicted_label = torch.argmax(outputs.logits).item()
    
    return bool(predicted_label)

def plot_confusion_matrix(TP, TN, FP, FN, labels=[1, 0]):
    cm = np.array([[TP, FP], [FN, TN]])
    fig, ax = plt.subplots()
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', xticklabels=labels, yticklabels=labels)
    # xlabel = Predicted, ylabel = Actual (bolded to highlight the axis labels)
    ax.set_xlabel('Predicted', fontsize=12)
    ax.set_ylabel('Actual', fontsize=12)
    ax.xaxis.set_label_position('top')
    ax.tick_params(axis='both', which='major', labelsize=10, labelbottom = False, bottom=False, top = True, labeltop=True)
    plt.show()

def compute_metrics(predictions, labels):
    accuracy = load_metric('accuracy')
    f1 = load_metric('f1')
    recall = load_metric('recall')
    confusion_matrix = load_metric('confusion_matrix')

    accuracy_result = accuracy.compute(predictions=predictions, references=labels)
    f1_result = f1.compute(predictions=predictions, references=labels, average='weighted')
    recall_result = recall.compute(predictions=predictions, references=labels, average='weighted')
    confusion_matrix_result = confusion_matrix.compute(predictions=predictions, references=labels)

    print(f"Accuracy: {accuracy_result['accuracy']}")
    print(f"F1 Score: {f1_result['f1']}")
    print(f"Recall: {recall_result['recall']}")
    print(confusion_matrix_result)

def main():
    dataset_path = "archive/final_test.csv"
    tokenized_dataset = load_and_tokenize_dataset(dataset_path)
    
    # Example usage of is_ai_generated function
    #text = "This is a sample text."
    ai_generated = is_ai_generated(dataset_path, model, tokenizer)
    print("AI-generated:", ai_generated)

    # To compute metrics, you would need to iterate over your dataset and collect predictions
    # This example shows how to use the function for a single text input
    #plot_confusion_matrix(100, 100, 100, 100)
    # compute_metrics(predictions, labels)

if __name__ == "__main__":
    main()
