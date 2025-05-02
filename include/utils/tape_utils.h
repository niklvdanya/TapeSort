#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace utils {

class TapeUtils {
public:
    static void generateRandomTape(const std::string& filename, size_t count, int32_t minValue = 0,
                                   int32_t maxValue = 1000000);

    static void printTapeContent(const std::string& filename, size_t maxElements = 20);

    static bool validateSorted(const std::string& filename);
};

}  // namespace utils