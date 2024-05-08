#ifndef FINITE_CONTEXT_MODEL_TRAINER_HPP_
#define FINITE_CONTEXT_MODEL_TRAINER_HPP_

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <numeric>

#include "finite_context_model.hpp"
#include "csv.hpp"

using namespace std;
using namespace csv;

class FiniteContextModelTrainer {
    public:
        size_t k;
        float smoothing_factor;
        string alphabet;
        bool ignore_case;
        uint8_t scaling_factor;
        unordered_map<string, FiniteContextModel> models;

        FiniteContextModelTrainer(const size_t &k, const float &smoothing_factor, const string &alphabet, const bool &ignore_case, const uint8_t &scaling_factor): k(k), smoothing_factor(smoothing_factor), alphabet(alphabet), ignore_case(ignore_case), scaling_factor(scaling_factor) {}

        void train(const string& input_file, const string& text_column, const string& label_column) {
            CSVReader reader(input_file);

            for (CSVRow& row: reader) {
                string text = row[text_column].get<>();
                string label = row[label_column].get<>();

                if (models.find(label) == models.end()) 
                    models.emplace(label, FiniteContextModel(k, smoothing_factor, alphabet, ignore_case, scaling_factor, label));

                models[label].update(text);
            }
        }

        void train(string& text, const string& label) {
            if (models.find(label) == models.end()) 
                models.emplace(label, FiniteContextModel(k, smoothing_factor, alphabet, ignore_case, scaling_factor, label));
            
            models[label].update(text);
        }

        void train(ifstream& input, const string& label) {
            if (models.find(label) == models.end()) 
                models.emplace(label, FiniteContextModel(k, smoothing_factor, alphabet, ignore_case, scaling_factor, label));

            models[label].update(input);
        }

        void save() {
            for (auto& [label, model]: models)
                model.save(label + ".bin");
        }

        void save(unordered_map<string, string> &filenames) {
            for (auto& [label, model]: models)
                model.save(filenames[label]);
        }
};

#endif // FINITE_CONTEXT_MODEL_TRAINER_HPP_