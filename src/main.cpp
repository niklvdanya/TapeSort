#include "file_tape.h"
#include "sorter.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file> [memory_limit_kb]" << std::endl;
        return 1;
    }
    
    size_t memoryLimitKB = 64;
    if (argc >= 4) {
        memoryLimitKB = std::stoul(argv[3]);
    }
    
    try {
        TapeConfig config;
        
        FileTape inputTape(argv[1], config);
        FileTape outputTape(argv[2], config);
        
        TapeSorter sorter(memoryLimitKB * 1024);
        
        std::cout << "Sorting data..." << std::endl;
        sorter.sort(inputTape, outputTape);
        std::cout << "Sorting completed!" << std::endl;
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}