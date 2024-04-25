#include <sstream>
#include <unordered_map>

using namespace std;

class WasChatted
{
    public:
        WasChatted(string gpt_file, string human_file, float alpha, int k);
        void detect(string input_file);
    private: 
        string input_file; 
        string gpt_file;
        string human_file;
        float alpha; 

        unordered_map<string, int> gpt_ngrams;

        void readInputFile();
        void readGPTFile();
        void readHumanFile();
        void countNGrams(string file, unordered_map<string, int>& ngrams, int k);
        float calculateDistance(unordered_map<string, int>& gpt_ngrams, unordered_map<string, int>& human_ngrams);
};

WasChatted::WasChatted(string gpt_file, string human_file, float alpha, int k)
{
    this->gpt_file = gpt_file;
    this->human_file = human_file;
    this->alpha = alpha;
}