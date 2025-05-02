#include "app/command_factory.h"
#include <iostream>
#include <vector>
#include <string>
#include <set>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: Command required" << std::endl;
        std::cerr << "Run 'help' for available commands" << std::endl;
        return 1;
    }
    
    app::CommandFactory factory;
    std::string commandName = argv[1];
    std::vector<std::string> args;
    
    static const std::set<std::string> validCommands = {
        "sort", "generate", "print", "validate", "help"
    };

    if (validCommands.find(commandName) == validCommands.end()) {
        args.push_back(commandName); 
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        commandName = "sort"; 
    } else {
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }
    }
    
    auto command = factory.createCommand(commandName, args);
    if (command) {
        try {
            return command->execute();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }
    
    std::cerr << "Run 'help' for available commands" << std::endl;
    return 1;
}