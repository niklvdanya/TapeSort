#include "file_tape.h"
#include <stdexcept>

FileTape::FileTape(const std::string& filename) : filename(filename) {
    file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    
    if (!file.is_open()) {
        file.open(filename, std::ios::binary | std::ios::out);
        file.close();
        file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    }
    
    file.seekg(0, std::ios::end);
    tapeSize = file.tellg() / sizeof(int32_t);
    file.seekg(0, std::ios::beg);
}

FileTape::~FileTape() {
    if (file.is_open()) {
        file.close();
    }
}

int32_t FileTape::read() {
    if (isEnd()) {
        throw std::runtime_error("Attempt to read past the end of tape");
    }
    
    int32_t value;
    file.seekg(position * sizeof(int32_t));
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    
    return value;
}

void FileTape::write(int32_t value) {
    file.seekp(position * sizeof(int32_t));
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
    file.flush();
    
    if (position >= tapeSize) {
        tapeSize = position + 1;
    }
}

void FileTape::rewind() {
    position = 0;
    file.seekg(0);
    file.seekp(0);
}

bool FileTape::moveNext() {
    if (isEnd()) {
        return false;
    }
    
    position++;
    return !isEnd();
}

bool FileTape::isEnd() const {
    return position >= tapeSize;
}