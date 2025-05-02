#pragma once

#include <cstdint>
#include <string>
#include <memory>

class ITape {
public:
    virtual ~ITape() = default;
    virtual int32_t read() = 0;
    virtual void write(int32_t value) = 0;
    virtual void rewind() = 0;
    virtual bool moveNext() = 0;
    virtual bool isEnd() const = 0;
    virtual size_t getPosition() const = 0;
};

struct TapeConfig {
    int readDelay = 1;
    int writeDelay = 2;
    int rewindDelay = 5;
    int shiftDelay = 1;
};