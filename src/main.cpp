#include "file_tape.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }
    
    try {
        FileTape inputTape(argv[1]);
        FileTape outputTape(argv[2]);
        inputTape.rewind();
        outputTape.rewind();
        
        while (!inputTape.isEnd()) {
            int32_t value = inputTape.read();
            outputTape.write(value);
            
            inputTape.moveNext();
            outputTape.moveNext();
        }
        
        std::cout << "Copy files" << std::endl;
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}