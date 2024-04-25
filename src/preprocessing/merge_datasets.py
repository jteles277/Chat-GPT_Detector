import argparse
import pandas as pd

def merge_datasets(*files):
    dfs = [pd.read_csv(file)[['text', 'label']] for file in files]
    merged_df = pd.concat(dfs)
    merged_df.drop_duplicates(subset="text", inplace=True)
    return merged_df

def parse_arguments():
    parser = argparse.ArgumentParser(description="Merge multiple datasets into one.")
    parser.add_argument("files", type=str, nargs="+", help="The paths to the dataset files.")
    parser.add_argument("output", type=str, help="The path to the output file.")
    return parser.parse_args()

def main():
    parsed_args = parse_arguments()
    if len(parsed_args.files) < 2:
        raise ValueError("At least two files must be provided.")
    merged_df = merge_datasets(*parsed_args.files)
    merged_df.to_csv(parsed_args.output, index=False)

# Example usages:
# - python3 preprocessing/merge_datasets.py archive/final_train.csv archive2/train_v4_drcat_01.csv archive/final_train_merged.csv

if __name__ == "__main__":
    main()