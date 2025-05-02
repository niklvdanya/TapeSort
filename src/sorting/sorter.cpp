#include "sorting/sorter.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>

namespace sorting {

TapeSorter::TapeSorter(size_t memoryLimit, const tape::Config& config,
                       std::shared_ptr<tape::TapeFactory> tapeFactory, std::string tempDir)
    : memoryLimit_(memoryLimit),
      config_(config),
      tapeFactory_(tapeFactory),
      tempDir_(std::move(tempDir)) {
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

        std::string chunkFilename = tempDir_ + "/chunk_" + std::to_string(chunkIndex++) + ".bin";
        chunkFiles.push_back(chunkFilename);

        auto chunkTape = tapeFactory_->createEmptyTape(chunkFilename, config_);
        for (const auto& value : buffer) {
            chunkTape->write(value);
            chunkTape->moveNext();
        }
    }

    return chunkFiles;
}

void TapeSorter::mergeChunks(const std::vector<std::string>& chunkFiles, tape::ITape& outputTape) {
    if (chunkFiles.empty())
        return;

    std::vector<std::unique_ptr<tape::ITape>> tapes;
    tapes.reserve(chunkFiles.size());

    for (const auto& filename : chunkFiles) {
        tapes.push_back(tapeFactory_->createTape(filename, config_));
        tapes.back()->rewind();
    }

    outputTape.rewind();

    struct Element {
        int32_t value;
        size_t tapeIndex;

        bool operator>(const Element& other) const { return value > other.value; }
    };

    std::priority_queue<Element, std::vector<Element>, std::greater<Element>> minHeap;

    // Initialize the heap with the first element from each tape
    for (size_t i = 0; i < tapes.size(); ++i) {
        if (!tapes[i]->isEnd()) {
            minHeap.push({tapes[i]->read(), i});
            tapes[i]->moveNext();
        }
    }

    // Merge the sorted chunks
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
}

void TapeSorter::sort(tape::ITape& inputTape, tape::ITape& outputTape) {
    auto startTime = std::chrono::high_resolution_clock::now();

    auto chunkFiles = splitIntoChunks(inputTape);
    mergeChunks(chunkFiles, outputTape);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Sort completed in " << totalDuration.count() << " ms" << std::endl;
}

}  // namespace sorting