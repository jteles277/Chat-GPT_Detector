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
};

#endif // APPROXIMATE_FINITE_CONTEXT_MODEL_HPP_