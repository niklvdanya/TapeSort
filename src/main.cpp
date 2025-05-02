#include "app/command_factory.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: Command required" << std::endl;
        std::cerr << "Run 'help' for available commands" << std::endl;
        return 1;
    }
    
    std::string commandName = argv[1];
    
    std::vector<std::string> args;
    for (int i = 2; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    app::CommandFactory factory;
    
    if (factory.isCommandRegistered(commandName)) {
        auto command = factory.createCommand(commandName, args);
        if (command) {
            try {
                return command->execute();
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                return 1;
            }
        }
    }
    
    if (commandName != "help") {
        std::cerr << "Unknown command: " << commandName << std::endl;
        std::cerr << "Run 'help' for available commands" << std::endl;
    }
    
    auto helpCommand = factory.createCommand("help", {});
    if (helpCommand) {
        return helpCommand->execute();
    }
    
    return 1;
}