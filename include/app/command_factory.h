#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace app {

class Command {
public:
    virtual ~Command() = default;
    virtual int execute() = 0;
    virtual std::string getDescription() const = 0;
};

class CommandFactory {
private:
    std::unordered_map<std::string,
                       std::function<std::unique_ptr<Command>(const std::vector<std::string>&)>>
        registry_;

public:
    CommandFactory();

    std::unique_ptr<Command> createCommand(const std::string& name,
                                           const std::vector<std::string>& args);
    bool isCommandRegistered(const std::string& name) const;
    std::vector<std::string> getAvailableCommands() const;
};

}  // namespace app