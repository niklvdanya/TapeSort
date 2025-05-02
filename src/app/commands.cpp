#include "app/commands.h"

namespace app {

bool BaseCommand::checkArgsCount(size_t minCount) const {
    if (args_.size() < minCount) {
        std::cerr << "Error: Not enough arguments" << std::endl;
        return false;
    }
    return true;
}

std::shared_ptr<tape::TapeFactory> SortCommand::createTapeFactory() const {
    return std::make_shared<tape::FileTapeFactory>();
}

int SortCommand::execute() {
    if (!checkArgsCount(2)) {
        std::cerr << getDescription() << std::endl;
        return 1;
    }

    std::string inputFile = args_[0];
    std::string outputFile = args_[1];

    size_t memoryLimitKB = 64;
    if (args_.size() >= 3) {
        try {
            memoryLimitKB = std::stoul(args_[2]);
        } catch (const std::exception&) {
            std::cerr << "Error: Invalid memory limit: " << args_[2] << std::endl;
            return 1;
        }
    }

    std::string configFile = "config/tape_config.txt";
    if (args_.size() >= 4) {
        configFile = args_[3];
    }

    std::string tempDir = "tmp";

    if (!std::filesystem::exists(tempDir)) {
        std::filesystem::create_directory(tempDir);
    }

    tape::Config config;

    auto optionalConfig = tape::Config::fromFile(configFile);
    if (optionalConfig) {
        config = *optionalConfig;
    }

    try {
        auto tapeFactory = createTapeFactory();
        auto inputTape = tapeFactory->createTape(inputFile, config);
        auto outputTape = tapeFactory->createEmptyTape(outputFile, config);

        sorting::TapeSorter sorter(memoryLimitKB * 1024, config, tapeFactory, tempDir);
        sorter.sort(*inputTape, *outputTape);

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int GenerateCommand::execute() {
    if (!checkArgsCount(2)) {
        std::cerr << getDescription() << std::endl;
        return 1;
    }

    std::string outputFile = args_[0];
    size_t count;

    try {
        count = std::stoul(args_[1]);
    } catch (const std::exception&) {
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
    if (!checkArgsCount(1)) {
        std::cerr << getDescription() << std::endl;
        return 1;
    }

    std::string filename = args_[0];
    size_t maxElements = 20;

    if (args_.size() >= 2) {
        try {
            maxElements = std::stoul(args_[1]);
        } catch (const std::exception&) {
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
    if (!checkArgsCount(1)) {
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
    std::cout << "Tape Sort - Commands:" << std::endl;

    CommandFactory factory;
    for (const auto& cmd : factory.getAvailableCommands()) {
        std::unique_ptr<Command> command = factory.createCommand(cmd, {});
        if (command) {
            std::cout << "  " << command->getDescription() << std::endl;
        }
    }

    return 0;
}

}  // namespace app