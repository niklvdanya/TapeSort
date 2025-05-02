#include "sorting/sorter.h"
#include "tape/file_tape.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <queue>
#include <iostream>
#include <chrono>

namespace sorting {

TapeSorter::TapeSorter(size_t memoryLimit, const tape::Config& config, std::string tempDir) 
    : memoryLimit_(memoryLimit), config_(config), tempDir_(std::move(tempDir)) {
    
    if (!std::filesystem::exists(tempDir_)) {
        std::filesystem::create_directory(tempDir_);
    }
}

std::vector<std::string> TapeSorter::splitIntoChunks(tape::ITape& inputTape) {
    std::vector<std::string> chunkFiles;
    size_t elementsInMemory = memoryLimit_ / sizeof(int32_t);
    
    if (elementsInMemory == 0) {
        elementsInMemory = 1;
    }
    
    std::cout << "Memory limit: " << memoryLimit_ << " bytes (" << elementsInMemory << " elements)" << std::endl;
    
    inputTape.rewind();
    
    size_t chunkIndex = 0;
    size_t totalElements = 0;
    
    while (!inputTape.isEnd()) {
        std::vector<int32_t> buffer;
        buffer.reserve(elementsInMemory);
        
        for (size_t i = 0; i < elementsInMemory && !inputTape.isEnd(); ++i) {
            buffer.push_back(inputTape.read());
            inputTape.moveNext();
            totalElements++;
        }
        
        std::cout << "Sorting chunk " << chunkIndex << " with " << buffer.size() << " elements" << std::endl;
        std::sort(buffer.begin(), buffer.end());
        
        std::string chunkFilename = tempDir_ + "/chunk_" + std::to_string(chunkIndex++) + ".bin";
        chunkFiles.push_back(chunkFilename);
        
        auto chunkTape = tape::FileTape::createEmpty(chunkFilename, config_);
        for (const auto& value : buffer) {
            chunkTape->write(value);
            chunkTape->moveNext();
        }
    }
    
    std::cout << "Total elements: " << totalElements << std::endl;
    std::cout << "Created " << chunkFiles.size() << " sorted chunks" << std::endl;
    
    return chunkFiles;
}

void TapeSorter::mergeChunks(const std::vector<std::string>& chunkFiles, tape::ITape& outputTape) {
    if (chunkFiles.empty()) return;
    
    std::cout << "Merging " << chunkFiles.size() << " chunks..." << std::endl;
    
    std::vector<std::unique_ptr<tape::FileTape>> tapes;
    tapes.reserve(chunkFiles.size());
    
    for (const auto& filename : chunkFiles) {
        tapes.push_back(tape::FileTape::createEmpty(filename, config_));
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
    
    size_t elementsMerged = 0;
    
    while (!minHeap.empty()) {
        auto minElement = minHeap.top();
        minHeap.pop();
        
        outputTape.write(minElement.value);
        outputTape.moveNext();
        elementsMerged++;
        
        if (elementsMerged % 1000 == 0) {
            std::cout << "Merged " << elementsMerged << " elements so far" << std::endl;
        }
        
        if (!tapes[minElement.tapeIndex]->isEnd()) {
            minHeap.push({tapes[minElement.tapeIndex]->read(), minElement.tapeIndex});
            tapes[minElement.tapeIndex]->moveNext();
        }
    }
    
    std::cout << "Total elements merged: " << elementsMerged << std::endl;
}

void TapeSorter::sort(tape::ITape& inputTape, tape::ITape& outputTape) {
    std::cout << "Starting sort operation..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto chunkFiles = splitIntoChunks(inputTape);
    
    auto splitTime = std::chrono::high_resolution_clock::now();
    auto splitDuration = std::chrono::duration_cast<std::chrono::milliseconds>(splitTime - startTime);
    std::cout << "Split phase completed in " << splitDuration.count() << " ms" << std::endl;
    
    mergeChunks(chunkFiles, outputTape);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto mergeDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - splitTime);
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Merge phase completed in " << mergeDuration.count() << " ms" << std::endl;
    
    std::cout << "Total sort time: " << totalDuration.count() << " ms" << std::endl;
}

} // namespace sorting