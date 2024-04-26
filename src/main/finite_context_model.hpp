#ifndef FINITE_CONTEXT_MODEL_HPP_
#define FINITE_CONTEXT_MODEL_HPP_

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <numeric>
#include <cmath>

#include "circular_buffer.hpp"

using namespace std;

class FiniteContextModel {
    
    struct EventMap {
        unordered_map<char, uint32_t> events;
        uint32_t total;
    };

    private:
        void increment(EventMap &counts, const char &event) {
            counts.events[event]++;
            counts.total++;
        }

    public:
        size_t k;
        float smoothing_factor;
        unordered_set<char> alphabet;
        bool ignore_case;
        string id;
        unordered_map<string, EventMap> context_counts;

        FiniteContextModel(): k(0), smoothing_factor(0), ignore_case(false) {}
        
        FiniteContextModel(const size_t &k, const float &smoothing_factor, const string &alphabet_, const bool &ignore_case, const string &id = ""): k(k), smoothing_factor(smoothing_factor), ignore_case(ignore_case), id(id) {
            for (char c : alphabet_) alphabet.insert(c);
        }

        FiniteContextModel(const string& input_file) {
            load(input_file);
        }

        void update(ifstream &input) {
            char c;
            circular_buffer<char> buffer(k);

            while (buffer.size() < k && input.get(c)) {
                if (is_valid_char(c)) 
                    buffer.put(c);
            }

            while (input.get(c)) {
                if (is_valid_char(c)) {
                    increment(context_counts[buffer], c);
                    buffer.put(c);
                }
            }
        }

        void update(string &input) {
            size_t i = 0;
            circular_buffer<char> buffer(k);

            while (buffer.size() < k && i < input.size()) {
                if (is_valid_char(input[i]))
                    buffer.put(input[i]);
                i++;
            }

            while (i < input.size()) {
                if (is_valid_char(input[i])) {
                    increment(context_counts[buffer], input[i]);
                    buffer.put(input[i]);
                }
                i++;
            }
        }

        uint32_t count(const string &context, const char &event) {
            return context_counts[context].events[event];
        }

        uint32_t count(const string &context) {
            return context_counts[context].total;
        }

        uint32_t count() {
            return accumulate(context_counts.begin(), context_counts.end(), 0, 
                [](uint32_t sum, const pair<string, EventMap> &context_count) {
                    return sum + context_count.second.total;
                });
        }

        float probability(const string &context, const char &event) {
            return (count(context, event) + smoothing_factor) / (count(context) + alphabet.size() * smoothing_factor);
        }

        float estimate_bits(const string &context, const char &event) {
            return -log2(probability(context, event));
        }

        float estimate_bits(ifstream &input, const bool &update = false) {
            float bits = 0;

            char c;
            circular_buffer<char> buffer(k);

            while (buffer.size() < k && input.get(c)) {
                if (is_valid_char(c)) 
                    buffer.put(c);
            }

            while (input.get(c)) {
                if (is_valid_char(c)) {
                    bits += estimate_bits(buffer, c);
                    if (update) increment(context_counts[buffer], c);
                    buffer.put(c);
                }
            }

            return bits;
        }
        
        float estimate_bits(const string &text, const bool &update = false) {
            float bits = 0;

            char c;
            size_t i = 0;
            circular_buffer<char> buffer(k);

            while (buffer.size() < k && i < text.size()) {
                c = text[i];
                if (is_valid_char(c)) 
                    buffer.put(c);
                i++;
            }

            while (i < text.size()) {
                c = text[i];
                if (is_valid_char(c)) {
                    bits += estimate_bits(buffer, c);
                    if (update) increment(context_counts[buffer], c);
                    buffer.put(c);
                }
                i++;
            }

            return bits;
        }

        void load(const string& input_file) {
            ifstream input(input_file, ios::binary);

            size_t id_size;
            input.read((char*)&id_size, sizeof(id_size));

            id.resize(id_size);
            input.read(&id[0], id_size);

            input.read((char*)&k, sizeof(k));
            input.read((char*)&smoothing_factor, sizeof(smoothing_factor));
            input.read((char*)&ignore_case, sizeof(ignore_case));

            size_t alphabet_size;
            input.read((char*)&alphabet_size, sizeof(alphabet_size));

            for (size_t i = 0; i < alphabet_size; i++) {
                char c;
                input.read(&c, sizeof(c));
                alphabet.insert(c);
            }

            size_t context_counts_size;
            input.read((char*)&context_counts_size, sizeof(context_counts_size));

            for (size_t i = 0; i < context_counts_size; i++) {
                size_t context_size;
                input.read((char*)&context_size, sizeof(context_size));

                string context;
                context.resize(context_size);
                input.read(&context[0], context_size);

                size_t events_size;
                input.read((char*)&events_size, sizeof(events_size));

                EventMap counts;
                for (size_t j = 0; j < events_size; j++) {
                    char event;
                    uint32_t count;
                    input.read(&event, sizeof(event));
                    input.read((char*)&count, sizeof(count));
                    counts.events[event] = count;
                }

                input.read((char*)&counts.total, sizeof(counts.total));
                context_counts[context] = counts;
            }
            
            input.close();
        }

        void save(const string &output_file) {
            ofstream output(output_file, ios::binary);

            size_t id_size = id.size();
            output.write((char*)&id_size, sizeof(id_size));
            output.write(id.c_str(), id.size());

            output.write((char*)&k, sizeof(k));
            output.write((char*)&smoothing_factor, sizeof(smoothing_factor));
            output.write((char*)&ignore_case, sizeof(ignore_case));

            size_t alphabet_size = alphabet.size();
            output.write((char*)&alphabet_size, sizeof(alphabet_size));

            for (char c : alphabet) output.write(&c, sizeof(c));

            size_t context_counts_size = context_counts.size();
            output.write((char*)&context_counts_size, sizeof(context_counts_size));

            for (const auto &context_count : context_counts) {
                size_t context_size = context_count.first.size();
                output.write((char*)&context_size, sizeof(context_size));
                output.write(context_count.first.c_str(), context_count.first.size());

                size_t events_size = context_count.second.events.size();
                output.write((char*)&events_size, sizeof(events_size));

                for (const auto &event : context_count.second.events) {
                    output.write(&event.first, sizeof(event.first));
                    output.write((char*)&event.second, sizeof(event.second));
                }

                output.write((char*)&context_count.second.total, sizeof(context_count.second.total));
            }
            
            output.close();
        }

        bool is_valid_char(char &c) {
            if (ignore_case) 
                c = toupper(c);
            return alphabet.find(c) != alphabet.end();
        }

        void reset() {
            context_counts.clear();
        }
};

#endif // FINITE_CONTEXT_MODEL_HPP_