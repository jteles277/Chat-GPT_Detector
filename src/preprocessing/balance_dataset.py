import argparse
import pandas as pd

def balance_by_row(df):
    min_class_count = df['label'].value_counts().min()
    return df.groupby('label').apply(lambda x: x.sample(min_class_count), include_groups=True).reset_index(drop=True)

def sample_by_char_count(data, target_count):
    sorted_data = data.sort_values(by='text', ascending=False, key=lambda x: x.str.len())
    char_count = sorted_data['text'].str.len().cumsum()
    sampled_indices = char_count[char_count <= target_count].index
    return data.loc[sampled_indices]

def balance_by_char_count(df):
    class_char_counts = df.groupby('label')['text'].apply(lambda x: x.str.len().sum())
    min_char_count = class_char_counts.min()
    return pd.concat([sample_by_char_count(df[df['label'] == label], min_char_count) for label in class_char_counts.index])

def balance(df, by="row"):
    if by == "row":
        return balance_by_row(df)
    if by == "char_count":
        return balance_by_char_count(df)
    raise ValueError("Invalid value for argument 'by'. Expected 'row' or 'char_count'.")

def parse_arguments():
    parser = argparse.ArgumentParser(description="Balance dataset by number of rows or number of characters.")
    parser.add_argument("file", type=str, help="Path to the dataset file.")
    parser.add_argument("output", type=str, help="Path to the output file.")
    parser.add_argument("--by", type=str, default="row", choices=["row", "char_count"], help="The method of balancing.")
    return parser.parse_args()
    
def main():
    parsed_args = parse_arguments()
    df = pd.read_csv(parsed_args.file)
    balanced_df = balance(df, by=parsed_args.by)
    balanced_df.to_csv(parsed_args.output, index=False)

# Example usages:
# - python3 src/preprocessing/balance_dataset.py archive/final_train.csv archive/final_train_balanced_by_row.csv --by row
# - python3 src/preprocessing/balance_dataset.py archive/final_train.csv archive/final_train_balanced_by_char_count.csv --by char_count

if __name__ == "__main__":
    main()


