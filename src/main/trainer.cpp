#include <iostream>
#include <vector>
#include <string>
#include <chrono>

#include "finite_context_model_trainer.hpp"

using namespace std;
using namespace chrono;

void printUsage(const char *argv0) {
    cout << "Usage: " << argv0 << " [-n num_labels] [-k order] [-s smoothing_factor] [-a alphabet] input_file+" << endl;
    cout << endl;
    cout << "Run the Trainer on the input file." << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -k order\t\t\tOrder for the Finite Context Model. (default: 5)" << endl;
    cout << "  -s smoothing_factor\t\tSmoothing factor for the Finite Context Model. (default: 1)" << endl;
    cout << "  -a alphabet\t\t\tAlphabet for the Finite Context Model. (default: abc...ABC...012...)" << endl;
    cout << "  -i\t\t\t\tIgnore case when training the model. The alphabet will be converted to uppercase. (default: false)" << endl;
    cout << "  -h\t\t\t\tDisplay this help message" << endl;
    cout << endl;
};


int main(int argc, char *argv[]) {
    int opt;
    
    bool ignore_case = false;
    size_t k = 5;
    float smoothing_factor = 1;
    string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    while ((opt = getopt(argc, argv, "k:s:a:ich")) != -1) {
        switch (opt) {
            case 'k':
                k = stoi(optarg);
                if (k < 1) {
                    cerr << "Order must be at least 1" << endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                smoothing_factor = stof(optarg);
                break;
            case 'a':
                alphabet = optarg;
                if (alphabet.empty()) {
                    cerr << "Alphabet cannot be empty" << endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 'i':
                ignore_case = true;
                break;
            case 'h':
                printf("Help\n");
                printUsage(argv[0]);
                exit(EXIT_SUCCESS);
            case '?':
                printf("Unknown option: %c\n", optopt);
                printUsage(argv[0]);
                exit(EXIT_FAILURE);
            case ':':
                printf("Missing argument for option: %c\n", optopt);
                printUsage(argv[0]);
                exit(EXIT_FAILURE);
            default:
                printf("Error parsing arguments\n");
                printUsage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    if (optind >= argc)
    {
        cerr << "Input file not provided" << endl;
        exit(EXIT_FAILURE);
    }

    vector<string> input_files(argv + optind, argv + argc);
    ApproximateFiniteContextModelTrainer trainer(k, smoothing_factor, alphabet, ignore_case);

    auto start_training = high_resolution_clock::now();
    for (string input_file: input_files)
        trainer.train(input_file, "text", "label");
    auto end_training = high_resolution_clock::now();
    cout << "Training time: " << duration_cast<seconds>(end_training - start_training).count() << " seconds" << endl;

    auto start_saving = high_resolution_clock::now();
    trainer.save();
    auto end_saving = high_resolution_clock::now();
    cout << "Saving time: " << duration_cast<seconds>(end_saving - start_saving).count() << " seconds" << endl;
};