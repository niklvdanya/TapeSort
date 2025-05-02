#include "app/commands.h"

namespace app {

int SortCommand::execute() {
    if (args_.size() < 2) {
        std::cerr << "Error: Not enough arguments" << std::endl;
        std::cerr << getDescription() << std::endl;
        return 1;
    }
    
    std::string inputFile = args_[0];
    std::string outputFile = args_[1];
    
    size_t memoryLimitKB = 64;
    if (args_.size() >= 3) {
        try {
            memoryLimitKB = std::stoul(args_[2]);
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid memory limit: " << args_[2] << std::endl;
            return 1;
        }
    }
    
    std::string configFile = "tape_config.txt";
    if (args_.size() >= 4) {
        configFile = args_[3];
    }
    
    std::string tempDir = "tmp";
    
    if (!std::filesystem::exists(tempDir)) {
        std::filesystem::create_directory(tempDir);
    }
    
    tape::Config config;
    std::string configPath = (std::filesystem::current_path().parent_path() / "config" / configFile).string();
    
    auto optionalConfig = tape::Config::fromFile(configPath);
    if (optionalConfig) {
        config = *optionalConfig;
        std::cout << "Loaded configuration from " << configPath << std::endl;
    } else {
        std::cout << "Using default configuration" << std::endl;
    }
    
    try {
        tape::FileTape inputTape(inputFile, config);
        tape::FileTape outputTape(outputFile, config);
        
        sorting::TapeSorter sorter(memoryLimitKB * 1024, config, tempDir);
        
        std::cout << "Sorting data with memory limit: " << memoryLimitKB << " KB" << std::endl;
        sorter.sort(inputTape, outputTape);
        std::cout << "Sorting completed successfully!" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int GenerateCommand::execute() {
    if (args_.size() < 2) {
        std::cerr << "Error: Not enough arguments" << std::endl;
        std::cerr << getDescription() << std::endl;
        return 1;
    }
    
    std::string outputFile = args_[0];
    size_t count;
    
    try {
        count = std::stoul(args_[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid count: " << args_[1] << std::endl;
        return 1;
    }
    
    try {
        utils::TapeUtils::generateRandomTape(outputFile, count);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int PrintCommand::execute() {
    if (args_.empty()) {
        std::cerr << "Error: Not enough arguments" << std::endl;
        std::cerr << getDescription() << std::endl;
        return 1;
    }
    
    std::string filename = args_[0];
    size_t maxElements = 20;
    
    if (args_.size() >= 2) {
        try {
            maxElements = std::stoul(args_[1]);
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid max_elements: " << args_[1] << std::endl;
            return 1;
        }
    }
    
    try {
        utils::TapeUtils::printTapeContent(filename, maxElements);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int ValidateCommand::execute() {
    if (args_.empty()) {
        std::cerr << "Error: Not enough arguments" << std::endl;
        std::cerr << getDescription() << std::endl;
        return 1;
    }
    
    std::string filename = args_[0];
    
    try {
        bool isSorted = utils::TapeUtils::validateSorted(filename);
        return isSorted ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int HelpCommand::execute() {
    std::cout << "Tape Sort - Utility for sorting data with limited memory" << std::endl;
    std::cout << "Commands:" << std::endl;
    
    CommandFactory factory;
    for (const auto& cmd : factory.getAvailableCommands()) {
        std::unique_ptr<Command> command = factory.createCommand(cmd, {});
        if (command) {
            std::cout << "  " << command->getDescription() << std::endl;
        }
    }
    
    return 0;
}

} // namespace app