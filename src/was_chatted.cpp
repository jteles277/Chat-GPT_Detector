#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "detector.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    int opt;

    string gpt_file;
    string human_file; 
    string input_file;
    string alphabet;  
    float smoothing_factor = 1;
    int k = 1;

    while ((opt = getopt(argc, argv, "a:s:k:c:m:f:h")) != -1)
    {
        switch (opt)
        {
        case 'a':
            alphabet = optarg; 
            break; 
        case 's': 
            smoothing_factor = stof(optarg);
            break;
        case 'k':
            k = stoi(optarg);
            break;
        case 'm':
            human_file = optarg;
            break;
        case 'c':
            gpt_file = optarg;
            break;
        case 'f': 
            input_file = optarg;
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


    // Check if files are valid 
    if (gpt_file.empty() || human_file.empty())
    {
        printf("Error: Missing input files\n");
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }
}

void printUsage(char *name)
{
    printf("Usage: %s [OPTIONS] input_file\n", name);
    printf("Options:\n");
    printf("  -a <float>  Alpha value for the detector\n");
    printf("  -k <int>    Number of top-k elements to consider\n");
    printf("  -h          Print this help message\n");
}