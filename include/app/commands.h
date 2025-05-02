#pragma once

#include "app/command_factory.h"
#include "tape/config.h"
#include "tape/file_tape.h"
#include "tape/file_tape_factory.h"
#include "sorting/sorter.h"
#include "utils/tape_utils.h"
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <optional>
#include <memory>

namespace app {

class BaseCommand : public Command {
protected:
    std::vector<std::string> args_;
    
    bool checkArgsCount(size_t minCount) const;
    
public:
    explicit BaseCommand(std::vector<std::string> args) : args_(std::move(args)) {}
};

class SortCommand : public BaseCommand {
private:
    std::shared_ptr<tape::TapeFactory> createTapeFactory() const;
    
public:
    explicit SortCommand(std::vector<std::string> args) : BaseCommand(std::move(args)) {}
    
    int execute() override;
    
    std::string getDescription() const override {
        return "sort <input_file> <output_file> [memory_limit_kb] [config_file] - Sort data from input file to output file";
    }
};

class GenerateCommand : public BaseCommand {
public:
    explicit GenerateCommand(std::vector<std::string> args) : BaseCommand(std::move(args)) {}
    
    int execute() override;
    
    std::string getDescription() const override {
        return "generate <output_file> <count> - Generate random data";
    }
};

class PrintCommand : public BaseCommand {
public:
    explicit PrintCommand(std::vector<std::string> args) : BaseCommand(std::move(args)) {}
    
    int execute() override;
    
    std::string getDescription() const override {
        return "print <file> [max_elements] - Print contents of a file";
    }
};

class ValidateCommand : public BaseCommand {
public:
    explicit ValidateCommand(std::vector<std::string> args) : BaseCommand(std::move(args)) {}
    
    int execute() override;
    
    std::string getDescription() const override {
        return "validate <file> - Check if a file is sorted";
    }
};

class HelpCommand : public BaseCommand {
public:
    explicit HelpCommand(std::vector<std::string> args) : BaseCommand(std::move(args)) {}
    
    int execute() override;
    
    std::string getDescription() const override {
        return "help - Show this help message";
    }
};

} // namespace app