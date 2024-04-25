import argparse
import pandas as pd

def sample_by_row(df, target_count):
    return df.sample(target_count)

def parse_arguments():
    parser = argparse.ArgumentParser(description="Sample dataset by number of rows")
    parser.add_argument("file", type=str, help="Path to the dataset file.")
    parser.add_argument("--n", type=int, help="Number of rows to sample.")
    parser.add_argument("output", type=str, help="Path to the output file.")
    return parser.parse_args()

def main():
    parsed_args = parse_arguments()
    df = pd.read_csv(parsed_args.file)
    n = min(parsed_args.n, len(df))     
    sample_df = sample_by_row(df, n)
    sample_df.to_csv(parsed_args.output, index=False)

# Example usage:
# - python3 src/preprocessing/sample_dataset.py --n 100 archive/final_train.csv archive/final_train_tiny.csv

if __name__ == "__main__":
    main()