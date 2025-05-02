#pragma once

#include "tape/config.h"
#include "tape/itape.h"

#include <memory>
#include <string>

namespace tape {

class TapeFactory {
public:
    virtual ~TapeFactory() = default;
    virtual std::unique_ptr<ITape> createTape(const std::string& filename,
                                              const Config& config) = 0;
    virtual std::unique_ptr<ITape> createEmptyTape(const std::string& filename,
                                                   const Config& config) = 0;
};

}  // namespace tape