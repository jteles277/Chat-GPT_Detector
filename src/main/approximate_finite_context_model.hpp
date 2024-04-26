#ifndef APPROXIMATE_FINITE_CONTEXT_MODEL_HPP_
#define APPROXIMATE_FINITE_CONTEXT_MODEL_HPP_

#include "finite_context_model.hpp"

using namespace std;

class ApproximateFiniteContextModel : public FiniteContextModel {
    private:
        void increment(EventMap &counts, const char &event) {
            if (counts.events[event] < b || (static_cast<double>(rand()) / RAND_MAX) < (1.0 / pow(1.0 + 1.0 / a, counts.events[event]))) {
                counts.events[event]++;
                counts.total++;
            }
        }
    public:
        uint32_t a;
        uint32_t b;

        ApproximateFiniteContextModel(const size_t &k, const float &smoothing_factor, const string &alphabet_, const bool &ignore_case, const uint32_t &a, const uint32_t &b, const string &id = ""): FiniteContextModel(k, smoothing_factor, alphabet_, ignore_case, id) {}

        uint32_t count(const string &context, const char &event) {
            if (context_counts[context].events[event] > b)
                return a * (pow(1.0 + 1.0 / a, context_counts[context].events[event]) - 1.0);
            return context_counts[context].events[event];
        }

        uint32_t count(const string &context) {
            if (context_counts[context].total > b)
                return a * (pow(1.0 + 1.0 / a, context_counts[context].total) - 1.0);
            return context_counts[context].total;
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
            input.read((char*)&a, sizeof(a));
            input.read((char*)&b, sizeof(b));

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
            output.write((char*)&a, sizeof(a));
            output.write((char*)&b, sizeof(b));

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
};

#endif // APPROXIMATE_FINITE_CONTEXT_MODEL_HPP_