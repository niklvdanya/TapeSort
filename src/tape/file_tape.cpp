#include "tape/file_tape.h"
#include <stdexcept>
#include <thread>
#include <chrono>

namespace tape {

FileTape::FileTape(const std::string& filename, const Config& config) 
    : filename_(filename), config_(config) {
    file_.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    
    if (!file_.is_open()) {
        file_.open(filename, std::ios::binary | std::ios::out);
        file_.close();
        file_.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    }
    
    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    
    file_.seekg(0, std::ios::end);
    tapeSize_ = file_.tellg() / sizeof(int32_t);
    file_.seekg(0, std::ios::beg);
}

FileTape::~FileTape() {
    if (file_.is_open()) {
        file_.close();
    }
}

FileTape::FileTape(FileTape&& other) noexcept
    : filename_(std::move(other.filename_)), 
      position_(other.position_), 
      tapeSize_(other.tapeSize_), 
      config_(other.config_) {
    
    if (file_.is_open()) {
        file_.close();
    }
    
    file_.swap(other.file_);

    other.position_ = 0;
    other.tapeSize_ = 0;
}

FileTape& FileTape::operator=(FileTape&& other) noexcept {
    if (this != &other) {
        if (file_.is_open()) {
            file_.close();
        }
        
        filename_ = std::move(other.filename_);
        position_ = other.position_;
        tapeSize_ = other.tapeSize_;
        config_ = other.config_;
        file_.swap(other.file_);
        
        other.position_ = 0;
        other.tapeSize_ = 0;
    }
    return *this;
}

void FileTape::simulateDelay(int milliseconds) const {
    if (milliseconds > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
}

int32_t FileTape::read() {
    if (isEnd()) {
        throw std::runtime_error("Attempt to read past the end of tape");
    }
    
    int32_t value;
    file_.seekg(position_ * sizeof(int32_t));
    file_.read(reinterpret_cast<char*>(&value), sizeof(value));
    
    if (file_.fail()) {
        throw std::runtime_error("Failed to read from file: " + filename_);
    }
    
    simulateDelay(config_.readDelay());
    return value;
}

void FileTape::write(int32_t value) {
    file_.seekp(position_ * sizeof(int32_t));
    file_.write(reinterpret_cast<const char*>(&value), sizeof(value));
    file_.flush();
    
    if (file_.fail()) {
        throw std::runtime_error("Failed to write to file: " + filename_);
    }
    
    if (position_ >= tapeSize_) {
        tapeSize_ = position_ + 1;
    }
    
    simulateDelay(config_.writeDelay());
}

void FileTape::rewind() {
    position_ = 0;
    file_.seekg(0);
    file_.seekp(0);
    
    simulateDelay(config_.rewindDelay());
}

bool FileTape::moveNext() {
    if (isEnd()) {
        return false;
    }
    
    position_++;
    simulateDelay(config_.shiftDelay());
    return !isEnd();
}

bool FileTape::isEnd() const {
    return position_ >= tapeSize_;
}

size_t FileTape::getPosition() const {
    return position_;
}

std::unique_ptr<FileTape> FileTape::createEmpty(const std::string& filename, const Config& config) {
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to create empty file: " + filename);
    }
    file.close();
    
    return std::make_unique<FileTape>(filename, config);
}

} // namespace tape