#include "sorter.h"
#include <vector>
#include <algorithm>

TapeSorter::TapeSorter(size_t memoryLimit) : memoryLimit(memoryLimit) {
}

void TapeSorter::sort(ITape& inputTape, ITape& outputTape) {
    size_t elementsInMemory = memoryLimit / sizeof(int32_t);
    
    std::vector<int32_t> buffer;
    buffer.reserve(elementsInMemory);
    
    inputTape.rewind();
    
    while (!inputTape.isEnd()) {
        buffer.push_back(inputTape.read());
        inputTape.moveNext();
        
        if (buffer.size() >= elementsInMemory || inputTape.isEnd()) {
            std::sort(buffer.begin(), buffer.end());
            
            outputTape.rewind();
            for (const auto& value : buffer) {
                outputTape.write(value);
                outputTape.moveNext();
            }
            
            buffer.clear();
        }
    }
}