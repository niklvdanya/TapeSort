#include "tape/config.h"

namespace tape {

std::optional<Config> Config::fromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << filename << std::endl;
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
        }
        else if (param == "writeDelay") {
            config.writeDelay_ = value;
            parametersRead++;
        }
        else if (param == "rewindDelay") {
            config.rewindDelay_ = value;
            parametersRead++;
        }
        else if (param == "shiftDelay") {
            config.shiftDelay_ = value;
            parametersRead++;
        }
        else {
            std::cerr << "Unknown parameter in config: " << param << std::endl;
        }
    }
    
    if (parametersRead > 0) {
        std::cout << "Loaded " << parametersRead << " parameters from config" << std::endl;
        return config;
    }
    
    std::cerr << "No valid parameters found in config file" << std::endl;
    return std::nullopt;
}

} // namespace tape