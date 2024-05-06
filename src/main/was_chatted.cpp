#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <chrono>

#include "finite_context_model_evaluator.hpp"

using namespace std;
using namespace chrono;

void print_usage(const char *argv0) {
    cout << "Usage: " << argv0 << " [-m model_file+] input_file+" << endl;
    cout << endl;
    cout << "Run the was_chatted program on the input file(s) using the model file(s)." << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -m model_file+\t\tModel file(s) for the Evaluator." << endl;
    cout << "  -h\t\t\t\tDisplay this help message" << endl;
    cout << endl;
}

int main(int argc, char *argv[]) {
    int opt;

    vector<string> model_files;

    while ((opt = getopt(argc, argv, "m:h")) != -1) {
        switch (opt) {
            case 'm':
                model_files.push_back(optarg);
                break;
            case 'h':
                printf("Help\n");
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
            case '?':
                printf("Unknown option: %c\n", optopt);
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            case ':':
                printf("Missing argument for option: %c\n", optopt);
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            default:
                printf("Error parsing arguments\n");
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (model_files.size() < 2)
    {
        cerr << "At least two model files must be provided" << endl;
        exit(EXIT_FAILURE);
    }

    if (optind >= argc)
    {
        cerr << "Input file not provided" << endl;
        exit(EXIT_FAILURE);
    }

    vector<string> input_files(argv + optind, argv + argc);

    auto start_loading = high_resolution_clock::now();

    FiniteContextModelEvaluator evaluator(model_files);

    auto end_loading = high_resolution_clock::now();

    cout << "Loading time: " << fixed << setprecision(6) << duration_cast<duration<double>>(end_loading - start_loading).count() << "s" << endl << endl;

    auto start_predicting = high_resolution_clock::now();

    cout << "Prediction results:" << endl;
    for (const string& input_file: input_files)
    {
        ifstream input(input_file);
        string label = evaluator.predict(input);
        
        cout << input_file << ": " << label << endl;
    }

    auto end_predicting = high_resolution_clock::now();

    
    cout << endl;
    cout << "Prediction time: " << fixed << setprecision(6) << duration_cast<duration<double>>(end_predicting - start_predicting).count() << "s" << endl;
    cout << "Average prediction time: " << fixed << setprecision(6) << duration_cast<duration<double>>(end_predicting - start_predicting).count() / input_files.size() << "s" << endl;
    cout << "Total time: " << fixed << setprecision(6) << duration_cast<duration<double>>(end_predicting - start_loading).count() << "s" << endl;
}