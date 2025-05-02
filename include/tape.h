#pragma once

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

struct TapeConfig {
    int readDelay = 0;
    int writeDelay = 0;
    int rewindDelay = 0;
    int shiftDelay = 0;

    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        std::string param;
        int value;
        while (file >> param >> value) {
            if (param == "readDelay")
                readDelay = value;
            else if (param == "writeDelay")
                writeDelay = value;
            else if (param == "rewindDelay")
                rewindDelay = value;
            else if (param == "shiftDelay")
                shiftDelay = value;
        }

        return true;
    }
};

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