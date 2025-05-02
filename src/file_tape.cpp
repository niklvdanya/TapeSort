#include "file_tape.h"
#include <stdexcept>
#include <iostream>
#include <thread>
#include <chrono>

FileTape::FileTape(const std::string& filename, const TapeConfig& config) 
    : filename(filename), config(config) {
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

void FileTape::simulateDelay(int milliseconds) const {
    if (milliseconds > 0) {
        std::cout << "Operation delay: " << milliseconds << " ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
}

int32_t FileTape::read() {
    if (isEnd()) {
        throw std::runtime_error("Attempt to read past the end of tape");
    }
    
    int32_t value;
    file.seekg(position * sizeof(int32_t));
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    
    simulateDelay(config.readDelay);
    return value;
}

void FileTape::write(int32_t value) {
    file.seekp(position * sizeof(int32_t));
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
    file.flush();
    
    if (position >= tapeSize) {
        tapeSize = position + 1;
    }
    
    simulateDelay(config.writeDelay);
}

void FileTape::rewind() {
    position = 0;
    file.seekg(0);
    file.seekp(0);
    
    simulateDelay(config.rewindDelay);
}

bool FileTape::moveNext() {
    if (isEnd()) {
        return false;
    }
    
    position++;
    simulateDelay(config.shiftDelay);
    return !isEnd();
}

bool FileTape::isEnd() const {
    return position >= tapeSize;
}

size_t FileTape::getPosition() const {
    return position;
}

std::unique_ptr<FileTape> FileTape::createEmpty(const std::string& filename, const TapeConfig& config) {
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    file.close();
    
    return std::make_unique<FileTape>(filename, config);
}