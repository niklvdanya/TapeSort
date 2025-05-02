#include "file_tape.h"
#include "sorter.h"
#include "utils.h"
#include <iostream>
#include <filesystem>
#include <chrono>

void printUsage(const char* programName) {
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << programName << " sort <input_file> <output_file> [memory_limit_kb] [config_file]" << std::endl;
    std::cout << "  " << programName << " generate <output_file> <count>" << std::endl;
    std::cout << "  " << programName << " print <file> [max_elements]" << std::endl;
    std::cout << "  " << programName << " validate <file>" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string command = argv[1];
    
    try {
        if (command == "sort" && argc >= 4) {
            std::string inputFile = argv[2];
            std::string outputFile = argv[3];
            
            size_t memoryLimitKB = 64;
            if (argc >= 5) {
                memoryLimitKB = std::stoul(argv[4]);
            }
            
            std::string configFile = "tape_config.txt";
            if (argc >= 6) {
                configFile = argv[5];
            }
            
            std::string tempDir = "tmp";
            
            if (!std::filesystem::exists(tempDir)) {
                std::filesystem::create_directory(tempDir);
            }
            
            TapeConfig config;
            std::string configPath = (std::filesystem::current_path().parent_path() / "config" / configFile).string();

            if (std::filesystem::exists(configPath)) {
                if (config.loadFromFile(configPath)) {
                    std::cout << "Loaded configuration from " << configPath << std::endl;
                } else {
                    std::cout << "Failed to parse configuration file: " << configPath << std::endl;
                }
            } else {
                std::cout << "Configuration file not found: " << configPath << std::endl;
                std::cout << "Using default configuration" << std::endl;
            }
            
            FileTape inputTape(inputFile, config);
            FileTape outputTape(outputFile, config);
            
            TapeSorter sorter(memoryLimitKB * 1024, config, tempDir);
            
            std::cout << "Sorting data with memory limit: " << memoryLimitKB << " KB" << std::endl;
            sorter.sort(inputTape, outputTape);
            std::cout << "Sorting completed successfully!" << std::endl;
        }
        else if (command == "generate" && argc >= 4) {
            std::string outputFile = argv[2];
            size_t count = std::stoul(argv[3]);
            
            TapeUtils::generateRandomTape(outputFile, count);
        }
        else if (command == "print" && argc >= 3) {
            std::string filename = argv[2];
            size_t maxElements = 20;
            
            if (argc >= 4) {
                maxElements = std::stoul(argv[3]);
            }
            
            TapeUtils::printTapeContent(filename, maxElements);
        }
        else if (command == "validate" && argc >= 3) {
            std::string filename = argv[2];
            
            TapeUtils::validateSorted(filename);
        }
        else {
            printUsage(argv[0]);
            return 1;
        }
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}