#pragma once

#include "tape/itape.h"
#include "tape/config.h"
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace sorting {

class TapeSorter {
private:
    size_t memoryLimit_;
    tape::Config config_;
    std::string tempDir_;
    
    std::vector<std::string> splitIntoChunks(tape::ITape& inputTape);
    void mergeChunks(const std::vector<std::string>& chunkFiles, tape::ITape& outputTape);

public:
    explicit TapeSorter(size_t memoryLimit, const tape::Config& config, std::string tempDir = "tmp");
    
    void sort(tape::ITape& inputTape, tape::ITape& outputTape);
};

} // namespace sorting