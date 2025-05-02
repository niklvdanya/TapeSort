#include "tape/config.h"

namespace tape {

std::optional<Config> Config::fromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return std::nullopt;
    }

    Config config;
    std::string param;
    int value;
    int parametersRead = 0;

    while (file >> param >> value) {
        if (param == "readDelay") {
            config.readDelay_ = value;
            parametersRead++;
        } else if (param == "writeDelay") {
            config.writeDelay_ = value;
            parametersRead++;
        } else if (param == "rewindDelay") {
            config.rewindDelay_ = value;
            parametersRead++;
        } else if (param == "shiftDelay") {
            config.shiftDelay_ = value;
            parametersRead++;
        }
    }

    if (parametersRead > 0) {
        return config;
    }

    return std::nullopt;
}

}  // namespace tape