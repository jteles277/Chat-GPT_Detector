import pandas as pd
from transformers import AutoTokenizer, AutoModelForSequenceClassification
import numpy as np
import torch
import seaborn as sns
import matplotlib.pyplot as plt

tokenizer = AutoTokenizer.from_pretrained("Hello-SimpleAI/chatgpt-detector-roberta")
model = AutoModelForSequenceClassification.from_pretrained("Hello-SimpleAI/chatgpt-detector-roberta")

def is_ai_generated(file_path, model, tokenizer):
    reader = pd.read_csv(file_path, chunksize=1)
    for chunk in reader: 
        row = chunk.iloc[0, 0]
        inputs = tokenizer(row, return_tensors="pt", max_length=512, truncation=True)

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


def main():
    dataset_path = "archive/final_test.csv"
     
    # Example usage of is_ai_generated function
    #text = "This is a sample text."
    ai_generated = is_ai_generated(dataset_path, model, tokenizer)
    print("AI-generated:", ai_generated)

    # To compute metrics, you would need to iterate over your dataset and collect predictions
    # This example shows how to use the function for a single text input
    #plot_confusion_matrix(100, 100, 100, 100)

if __name__ == "__main__":
    main()
