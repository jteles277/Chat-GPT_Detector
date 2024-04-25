# Chat-GPT_Detector

#### Notes:
- How different preprocessing techniques affect the performance of the model, e.g, remove accents, remove special characters, ignore case, etc.?

#### Commands:
- `g++ -Wall -O3 -o bin/trainer src/main/trainer.cpp`
- `g++ -Wall -O3 -o bin/evaluator src/main/evaluator.cpp`
- `./bin/trainer archive/final_train_balanced_by_char_count.csv`
- `./bin/evaluator -m 0.bin -m 1.bin archive/final_test.csv`
