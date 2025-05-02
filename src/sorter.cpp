#include "sorter.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <queue>

TapeSorter::TapeSorter(size_t memoryLimit, const TapeConfig& config, const std::string& tempDir) 
    : memoryLimit(memoryLimit), config(config), tempDir(tempDir) {
    
    if (!std::filesystem::exists(tempDir)) {
        std::filesystem::create_directory(tempDir);
    }
}

std::vector<std::string> TapeSorter::splitIntoChunks(ITape& inputTape) {
    std::vector<std::string> chunkFiles;
    size_t elementsInMemory = memoryLimit / sizeof(int32_t);
    
    inputTape.rewind();
    
    size_t chunkIndex = 0;
    while (!inputTape.isEnd()) {
        std::vector<int32_t> buffer;
        buffer.reserve(elementsInMemory);
        
        for (size_t i = 0; i < elementsInMemory && !inputTape.isEnd(); ++i) {
            buffer.push_back(inputTape.read());
            inputTape.moveNext();
        }
        
        std::sort(buffer.begin(), buffer.end());
        
        std::string chunkFilename = tempDir + "/chunk_" + std::to_string(chunkIndex++) + ".bin";
        chunkFiles.push_back(chunkFilename);
        
        auto chunkTape = FileTape::createEmpty(chunkFilename, config);
        for (const auto& value : buffer) {
            chunkTape->write(value);
            chunkTape->moveNext();
        }
    }
    
    return chunkFiles;
}

void TapeSorter::mergeChunks(const std::vector<std::string>& chunkFiles, ITape& outputTape) {
    if (chunkFiles.empty()) return;
    
    std::vector<std::unique_ptr<FileTape>> tapes;
    
    for (const auto& filename : chunkFiles) {
        tapes.push_back(std::make_unique<FileTape>(filename, config));
        tapes.back()->rewind();
    }
    
    outputTape.rewind();
    
    struct Element {
        int32_t value;
        size_t tapeIndex;
        
        bool operator>(const Element& other) const {
            return value > other.value;
        }
    };
    
    std::priority_queue<Element, std::vector<Element>, std::greater<Element>> minHeap;
    
    for (size_t i = 0; i < tapes.size(); ++i) {
        if (!tapes[i]->isEnd()) {
            minHeap.push({tapes[i]->read(), i});
            tapes[i]->moveNext();
        }
    }
    
    while (!minHeap.empty()) {
        auto minElement = minHeap.top();
        minHeap.pop();
        
        outputTape.write(minElement.value);
        outputTape.moveNext();
        
        if (!tapes[minElement.tapeIndex]->isEnd()) {
            minHeap.push({tapes[minElement.tapeIndex]->read(), minElement.tapeIndex});
            tapes[minElement.tapeIndex]->moveNext();
        }
    }
    
    for (const auto& filename : chunkFiles) {
        std::filesystem::remove(filename);
    }
}

void TapeSorter::sort(ITape& inputTape, ITape& outputTape) {
    auto chunkFiles = splitIntoChunks(inputTape);
    mergeChunks(chunkFiles, outputTape);
}