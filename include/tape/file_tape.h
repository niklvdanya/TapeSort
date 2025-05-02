#pragma once

#include "tape/itape.h"
#include "tape/config.h"
#include <string>
#include <fstream>
#include <memory>

namespace tape {

class FileTape final : public ITape {
private:
    std::fstream file_;
    std::string filename_;
    size_t position_ = 0;
    size_t tapeSize_ = 0;
    Config config_;
    
    void simulateDelay(int milliseconds) const;

public:
    FileTape(const std::string& filename, const Config& config);
    ~FileTape() override;
    
    FileTape(const FileTape&) = delete;
    FileTape& operator=(const FileTape&) = delete;
    
    FileTape(FileTape&&) noexcept;
    FileTape& operator=(FileTape&&) noexcept;
    
    int32_t read() override;
    void write(int32_t value) override;
    void rewind() override;
    bool moveNext() override;
    bool isEnd() const override;
    size_t getPosition() const override;
    
    static std::unique_ptr<FileTape> createEmpty(const std::string& filename, const Config& config);
};

} // namespace tape