#pragma once

#include "tape.h"
#include <string>
#include <fstream>

class FileTape : public ITape {
private:
    std::fstream file;
    std::string filename;
    size_t position = 0;
    size_t tapeSize = 0;

public:
    FileTape(const std::string& filename);
    ~FileTape() override;
    
    int32_t read() override;
    void write(int32_t value) override;
    void rewind() override;
    bool moveNext() override;
    bool isEnd() const override;
};