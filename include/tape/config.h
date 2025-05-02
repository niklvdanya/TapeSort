#pragma once

#include <string>
#include <optional>
#include <iostream>
#include <fstream>

namespace tape {

class Config {
private:
    int readDelay_ = 1;
    int writeDelay_ = 2;
    int rewindDelay_ = 5;
    int shiftDelay_ = 1;

public:
    Config() = default;
    
    Config(int readDelay, int writeDelay, int rewindDelay, int shiftDelay) noexcept
        : readDelay_(readDelay), writeDelay_(writeDelay), 
          rewindDelay_(rewindDelay), shiftDelay_(shiftDelay) {}
    
    static std::optional<Config> fromFile(const std::string& filename);
    
    int readDelay() const noexcept { return readDelay_; }
    int writeDelay() const noexcept { return writeDelay_; }
    int rewindDelay() const noexcept { return rewindDelay_; }
    int shiftDelay() const noexcept { return shiftDelay_; }
};

} // namespace tape