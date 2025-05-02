#include "tape/file_tape_factory.h"

namespace tape {

std::unique_ptr<ITape> FileTapeFactory::createTape(const std::string& filename, const Config& config) {
    return std::make_unique<FileTape>(filename, config);
}

std::unique_ptr<ITape> FileTapeFactory::createEmptyTape(const std::string& filename, const Config& config) {
    return FileTape::createEmpty(filename, config);
}

} // namespace tape