import argparse
from collections import Counter
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

sns.set_theme(style="whitegrid")

def plot_class_distribution_pie_chart(data, label=None):
    palette = sns.color_palette("bwr", 5)[1:5:3] 
    plt.pie(data, labels=data.index, autopct='%1.1f%%', startangle=140, textprops={'fontsize': 20}, colors=palette)
    plt.ylabel('')
    plt.title(label)

def plot_class_distribution_bar_chart(data, label=None):
    palette = sns.color_palette("bwr", 5)[1:5:3] 
    sns.barplot(x=data.index, y=data, palette=palette, hue=data.index, legend=False)
    plt.xlabel('label', fontsize=20)
    plt.ylabel('count', fontsize=20)
    plt.title(label)
    plt.xticks(rotation=0)
    plt.tick_params(axis='both', which='major', labelsize=18)
    plt.grid(linestyle='--')

def make_class_counts(df, by="row"):
    if by == "row":
        return df['label'].value_counts()
    if by == "char_count":
        return df.groupby('label')['text'].apply(lambda x: x.str.len().sum())
    raise ValueError("Invalid value for argument 'by'. Expected 'row' or 'char_count'.")

def plot_split_class_distribution(class_counts):
    plt.figure(figsize=(7, 7))
    plot_class_distribution_pie_chart(class_counts)
    plt.tight_layout()
    plt.show()

    plt.figure(figsize=(7, 7))
    plot_class_distribution_bar_chart(class_counts)
    plt.tight_layout()
    plt.show()

def plot_joint_class_distribution(class_counts):
    plt.figure(figsize=(14, 7))

    plt.subplot(1, 2, 1)
    plot_class_distribution_pie_chart(class_counts)

    plt.subplot(1, 2, 2)
    plot_class_distribution_bar_chart(class_counts)

    plt.tight_layout(w_pad=4)
    plt.show()

def plot_class_distribution(df, by="row", split=False):
    class_counts = make_class_counts(df, by=by)
    plot_split_class_distribution(class_counts) if split else plot_joint_class_distribution(class_counts)

def plot_char_distribution_bar_chart(data, label=None):
    palette = sns.color_palette("Set3", len(data))
    palette[-3], palette[-1] = palette[-1], palette[-3]
    data = {k if k != " " else "space": v for k, v in data.items()}
    sns.barplot(x=list(data.keys()), y=list(data.values()), palette=palette, hue=list(data.keys()), legend=False)
    plt.xlabel('char', fontsize=20)
    plt.ylabel('count', fontsize=20)
    plt.title(label)
    plt.xticks(rotation=0)
    plt.tick_params(axis='both', which='major', labelsize=18)
    plt.grid(linestyle='--')

def plot_split_char_distribution(class_char_counts):
    for label, char_counts in class_char_counts.items():
        plt.figure(figsize=(9, 7))
        plot_char_distribution_bar_chart(char_counts)
        plt.tight_layout()
        plt.show()

def plot_joint_char_distribution(char_counts):
    plt.figure(figsize=(18, 7))

    n_rows = len(char_counts) // 2 + len(char_counts) % 2
    n_cols = 2

    for i, (label, char_counts) in enumerate(char_counts.items()):
        plt.subplot(n_rows, n_cols, i + 1)
        plot_char_distribution_bar_chart(char_counts)

    plt.tight_layout(w_pad=4, h_pad=4)
    plt.show()

def plot_char_distribution(df, n=10, split=False):
    class_char_counts = {}
    for label, group in df.groupby('label'):
        char_counts = Counter(''.join(group['text']))
        top_n_chars = dict(char_counts.most_common(n))
        class_char_counts[label] = {**top_n_chars, 'other': sum(char_counts.values()) - sum(top_n_chars.values())}
    plot_split_char_distribution(class_char_counts) if split else plot_joint_char_distribution(class_char_counts)

def parse_arguments():
    parser = argparse.ArgumentParser(description="Analyze the distribution of the classes in the dataset.")
    parser.add_argument("file", type=str, help="The path to the dataset file.")
    parser.add_argument("--mode", type=str, default="class", choices=["class", "char"], help="The mode of analysis.")
    parser.add_argument("--by", type=str, default="row", choices=["row", "char_count"], help="The method of counting.")
    parser.add_argument("--split", action="store_true", help="Whether to split the analysis by class.")
    parser.add_argument("--n", type=int, default=10, help="The number of top characters to display.")
    return parser.parse_args()

def main():
    parsed_args = parse_arguments()
    df = pd.read_csv(parsed_args.file)
    if parsed_args.mode == "class":
        plot_class_distribution(df, by=parsed_args.by, split=parsed_args.split)
    if parsed_args.mode == "char":
        plot_char_distribution(df, n=parsed_args.n, split=parsed_args.split)

# Example usages:
# - python3 src/preprocessing/data_analysis.py archive/final_train.csv --mode class --by row --split
# - python3 src/preprocessing/data_analysis.py archive/final_train.csv --mode char --n 10 --split
# - python3 src/preprocessing/data_analysis.py archive/final_train.csv --mode class --by char_count
# - python3 src/preprocessing/data_analysis.py archive/final_train_balanced_by_row.csv --mode class --by row
# - python3 src/preprocessing/data_analysis.py archive/final_train_balanced_by_char_count.csv --mode class --by char_count
# - python3 src/preprocessing/data_analysis.py archive/final_train_balanced_by_char_count.csv --mode char --n 10

if __name__ == "__main__":
    main()