#!/bin/bash

# Define the values of k and s
k_values=(1 2)
s_values=(0.5 0.75 1 1.25 1.5)
a_value="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

# Specify the output directory for the trained models and evaluation results
output_directory="trained_models"
evaluation_results="evaluation_results_2.txt"

# Create the output directory if it doesn't exist
mkdir -p "$output_directory"

# Remove existing evaluation results file if exists
rm -f "$evaluation_results"

# Iterate over the combinations of k and s
for k in "${k_values[@]}"; do
    for s in "${s_values[@]}"; do
        # Run the trainer command with the specified parameters and capture the execution time
        start_time=$(date +%s.%N)
        ./bin/trainer -k "$k" -s "$s" -a "$a_value" archive/final_train_balanced_by_char_count.csv 
        end_time=$(date +%s.%N)
        execution_time=$(echo "$end_time - $start_time" | bc)
        
        # Run the evaluator command with the generated model filenames and capture the evaluation output
        evaluation_output="$(
            ./bin/evaluator -m "${output_directory}/0_${k}_$(printf "%.6f" "$s").bin" -m "${output_directory}/1_${k}_$(printf "%.6f" "$s").bin" archive/final_test.csv
        )"
        
        # Save the evaluation output along with the corresponding k, s, and execution time values
        echo "k = $k, s = $s, Training time: $execution_time seconds" >> "$evaluation_results"
        echo "$evaluation_output" >> "$evaluation_results"
        echo "" >> "$evaluation_results"  # Add a blank line for better readability
    done
done
