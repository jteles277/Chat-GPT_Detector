#ifndef WASCHATTED_H
#define WASCHATTED_H

#include <unordered_map>
#include <string>

class WasChatted
{
public:
    WasChatted(std::string gpt_file, std::string human_file, float alpha, int k);
    void detect(std::string input_file);

private:
    std::string input_file;
    std::string gpt_file;
    std::string human_file;
    float alpha;

    std::unordered_map<std::string, int> gpt_ngrams;

    void readInputFile();
    void readGPTFile();
    void readHumanFile();
    void countNGrams(std::string file, std::unordered_map<std::string, int>& ngrams, int k);
    float calculateDistance(std::unordered_map<std::string, int>& gpt_ngrams, std::unordered_map<std::string, int>& human_ngrams);
};
#endif 
