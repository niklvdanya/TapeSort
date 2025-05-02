#include "sorter.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <queue>
#include <iostream>

TapeSorter::TapeSorter(size_t memoryLimit, const TapeConfig& config, const std::string& tempDir) 
    : memoryLimit(memoryLimit), config(config), tempDir(tempDir) {
    
    if (!std::filesystem::exists(tempDir)) {
        std::filesystem::create_directory(tempDir);
    }
}

std::vector<std::string> TapeSorter::splitIntoChunks(ITape& inputTape) {
    std::vector<std::string> chunkFiles;
    size_t elementsInMemory = memoryLimit / sizeof(int32_t);
    
    if (elementsInMemory == 0) {
        elementsInMemory = 1;
    }
    
    std::cout << "Memory limit: " << memoryLimit << " bytes (" << elementsInMemory << " elements)" << std::endl;
    
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
        
        std::string chunkFilename = tempDir + "/chunk_" + std::to_string(chunkIndex++) + ".bin";
        chunkFiles.push_back(chunkFilename);
        
        auto chunkTape = FileTape::createEmpty(chunkFilename, config);
        for (const auto& value : buffer) {
            chunkTape->write(value);
            chunkTape->moveNext();
        }
    }
    
    std::cout << "Total elements: " << totalElements << std::endl;
    std::cout << "Created " << chunkFiles.size() << " sorted chunks" << std::endl;
    
    return chunkFiles;
}

void TapeSorter::mergeChunks(const std::vector<std::string>& chunkFiles, ITape& outputTape) {
    if (chunkFiles.empty()) return;
    
    std::cout << "Merging " << chunkFiles.size() << " chunks..." << std::endl;
    
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
    
    for (const auto& filename : chunkFiles) {
        std::filesystem::remove(filename);
    }
    
    std::cout << "Temporary files cleaned up" << std::endl;
}

void TapeSorter::sort(ITape& inputTape, ITape& outputTape) {
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