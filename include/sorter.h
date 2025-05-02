#pragma once

#include "tape.h"
#include <memory>

class TapeSorter {
private:
    size_t memoryLimit;

public:
    TapeSorter(size_t memoryLimit);
    
    void sort(ITape& inputTape, ITape& outputTape);
};