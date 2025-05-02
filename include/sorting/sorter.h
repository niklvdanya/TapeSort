#pragma once

#include "tape/config.h"
#include "tape/itape.h"
#include "tape/tape_factory.h"

#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace sorting {

class TapeSorter {
private:
    size_t memoryLimit_;
    tape::Config config_;
    std::string tempDir_;
    std::shared_ptr<tape::TapeFactory> tapeFactory_;

    std::vector<std::string> splitIntoChunks(tape::ITape& inputTape);
    void mergeChunks(const std::vector<std::string>& chunkFiles, tape::ITape& outputTape);

public:
    explicit TapeSorter(size_t memoryLimit, const tape::Config& config,
                        std::shared_ptr<tape::TapeFactory> tapeFactory,
                        std::string tempDir = "tmp");

    void sort(tape::ITape& inputTape, tape::ITape& outputTape);
};

}  // namespace sorting