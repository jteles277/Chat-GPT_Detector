#ifndef FINITE_CONTEXT_MODEL_EVALUATOR_HPP_
#define FINITE_CONTEXT_MODEL_EVALUATOR_HPP_

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <limits>
#include <numeric>

#include "finite_context_model.hpp"
#include "csv.hpp"

using namespace std;
using namespace csv;

class FiniteContextModelEvaluator {
    public:
        double bits = 0;
        unordered_map<string, FiniteContextModel> models;
        unordered_map<string, unordered_map<string, uint32_t>> confusion_matrix;

        FiniteContextModelEvaluator(const vector<string>& model_files) {
            for (const string& model_file: model_files) {
                FiniteContextModel model(model_file);
                models.emplace(model.id, model);
            }
        }

        FiniteContextModelEvaluator(const unordered_map<string, string>& model_files) {
            for (const auto& [label, model_file]: model_files)
                models.emplace(label, FiniteContextModel(model_file));
        }

        FiniteContextModelEvaluator(const unordered_map<string, FiniteContextModel>& models): models(models) {}

        void evaluate(const string& text, const string& label) {
            string predicted_label = predict(text);
            confusion_matrix[label][predicted_label]++;
        }

        void evaluate(const string& input_file, const string& text_column, const string& label_column) {
            CSVReader reader(input_file);

            for (CSVRow& row: reader) {
                string text = row[text_column].get<>();
                string label = row[label_column].get<>();

                string predicted_label = predict(text);
                confusion_matrix[label][predicted_label]++;
            }
        }

        string predict(ifstream& input_file) {
            float min_bits = numeric_limits<float>::max();
            string predicted_label;

            for (auto& [label, model]: models) {
                float bits = model.estimate_bits(input_file);
                if (bits < min_bits) {
                    min_bits = bits;
                    predicted_label = label;
                }
            }

            bits += min_bits;

            return predicted_label;
        }

        string predict(const string& text) {
            float min_bits = numeric_limits<float>::max();
            string predicted_label;

            for (auto& [label, model]: models) {
                float bits = model.estimate_bits(text);
                if (bits < min_bits) {
                    min_bits = bits;
                    predicted_label = label;
                }
            }

            bits += min_bits;

            return predicted_label;
        }

        uint32_t count(const string& label, const string& predicted_label) {
            return confusion_matrix[label][predicted_label];
        }

        uint32_t count(const string& label) {
            return accumulate(confusion_matrix[label].begin(), confusion_matrix[label].end(), 0,
                [](const uint32_t& sum, const auto& count) {
                    return sum + count.second;
                });
        }

        uint32_t count() {
            return accumulate(confusion_matrix.begin(), confusion_matrix.end(), 0,
                [](const uint32_t& sum, const auto& label_counts) {
                    return sum + accumulate(label_counts.second.begin(), label_counts.second.end(), 0,
                        [](const uint32_t& inner_sum, const auto& count) {
                            return inner_sum + count.second;
                        });
                });
        }

        uint32_t hits() {
            return accumulate(confusion_matrix.begin(), confusion_matrix.end(), 0,
                [](const uint32_t& sum, auto& label_counts) {
                    return sum + label_counts.second[label_counts.first];
                });
        }

        uint32_t misses() {
            return accumulate(confusion_matrix.begin(), confusion_matrix.end(), 0,
                [](const uint32_t& sum, const auto& label_counts) mutable {
                    return sum + accumulate(label_counts.second.begin(), label_counts.second.end(), 0,
                        [&label_counts](const uint32_t& inner_sum, const auto& count) {
                            if (count.first != label_counts.first)
                                return inner_sum + count.second;
                            return inner_sum;
                        });
                });
        }

        uint32_t true_positives(const string& label) {
            return confusion_matrix[label][label];
        }

        uint32_t false_positives(const string& label) {
            return accumulate(confusion_matrix.begin(), confusion_matrix.end(), 0,
                [&label](const uint32_t& sum, auto& label_counts) mutable {
                    if (label_counts.first != label)
                        return sum + label_counts.second[label];
                    return sum;
                });
        }

        uint32_t true_negatives(const string& label) {
            return accumulate(confusion_matrix.begin(), confusion_matrix.end(), 0,
                [&label](const uint32_t& sum, auto& label_counts) mutable {
                    if (label_counts.first != label)
                        return sum + label_counts.second[label_counts.first];
                    return sum;
                });
        }

        uint32_t false_negatives(const string& label) {
            return accumulate(confusion_matrix[label].begin(), confusion_matrix[label].end(), 0,
                [&label](const uint32_t& sum, const auto& count) mutable {
                    if (count.first != label)
                        return sum + count.second;
                    return sum;
                });
        }

        float recall(const string& label) {
            return true_positives(label) / (true_positives(label) + false_negatives(label));
        }

        float precision(const string& label) {
            return true_positives(label) / (true_positives(label) + false_positives(label));
        }

        float f_score(const string& label, float b = 1) {
            return (1 + b * b) * precision(label) * recall(label) / (b * b * precision(label) + recall(label));
        }

        float accuracy(const string& label) {
            return true_positives(label) / count(label);
        }

        float accuracy() {
            return static_cast<float>(hits()) / count();
        }

        float average_bits() {
            return bits / count();
        }   

        void summary() {
            vector<string> labels;
            for (const auto& [label, _]: confusion_matrix) labels.push_back(label);
            sort(labels.begin(), labels.end());

            cout << "Confusion Matrix (Actual\\Predicted):" << endl;

            for (const string& label: labels) cout << "\t" << label;
            cout << endl;

            for (const string& label: labels) {
                cout << label;
                for (const string& predicted_label: labels) cout << "\t" << confusion_matrix[label][predicted_label];
                cout << endl;
            }

            cout << endl;

            cout << "Total: " << count() << endl;
            cout << "Accuracy: " << accuracy() << endl;
            cout << "Total bits: " << bits << endl;
            cout << "Average bits: " << average_bits() << endl;
        }
};

#endif // FINITE_CONTEXT_MODEL_EVALUATOR_HPP_