#pragma once

#include "tape/tape_factory.h"
#include "tape/file_tape.h"

namespace tape {

class FileTapeFactory : public TapeFactory {
public:
    std::unique_ptr<ITape> createTape(const std::string& filename, const Config& config) override;
    std::unique_ptr<ITape> createEmptyTape(const std::string& filename, const Config& config) override;
};

} // namespace tape