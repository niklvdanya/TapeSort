#include "file_tape.h"
#include "sorter.h"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file> [memory_limit_kb]" << std::endl;
        return 1;
    }
    
    size_t memoryLimitKB = 64;
    if (argc >= 4) {
        memoryLimitKB = std::stoul(argv[3]);
    }
    
    std::string tempDir = "tmp";
    
    try {
        if (!std::filesystem::exists(tempDir)) {
            std::filesystem::create_directory(tempDir);
        }
        
        TapeConfig config;
        
        FileTape inputTape(argv[1], config);
        FileTape outputTape(argv[2], config);
        
        TapeSorter sorter(memoryLimitKB * 1024, config, tempDir);
        
        std::cout << "Sorting data with memory limit: " << memoryLimitKB << " KB" << std::endl;
        sorter.sort(inputTape, outputTape);
        std::cout << "Sorting completed successfully!" << std::endl;
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}