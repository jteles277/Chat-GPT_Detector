import argparse
import pandas as pd

def clean_dataset(df):
    pass

def parse_arguments():
    parser = argparse.ArgumentParser(description="Clean the dataset.")
    parser.add_argument("file", type=str, help="Path to the dataset file.")
    parser.add_argument("output", type=str, help="Path to the output file.")
    return parser.parse_args()

def main():
    parsed_args = parse_arguments()
    df = pd.read_csv(parsed_args.file)
    clean_df = clean_dataset(df)
    clean_df.to_csv(parsed_args.output, index=False)

# Example usage:
# - python3 src/preprocessing/clean_dataset.py archive/final_train.csv archive/final_train_cleaned.csv

if __name__ == "__main__":
    main()