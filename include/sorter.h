#pragma once

#include "tape.h"
#include "file_tape.h"
#include <memory>
#include <vector>
#include <string>

class TapeSorter {
private:
    size_t memoryLimit;
    TapeConfig config;
    std::string tempDir;
    
    std::vector<std::string> splitIntoChunks(ITape& inputTape);
    void mergeChunks(const std::vector<std::string>& chunkFiles, ITape& outputTape);

public:
    TapeSorter(size_t memoryLimit, const TapeConfig& config, const std::string& tempDir = "tmp");
    
    void sort(ITape& inputTape, ITape& outputTape);
};