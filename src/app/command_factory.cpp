#include "app/command_factory.h"

#include "app/commands.h"

#include <algorithm>

namespace app {

CommandFactory::CommandFactory() {
    registry_["sort"] = [](const std::vector<std::string>& args) {
        return std::make_unique<SortCommand>(args);
    };

    registry_["generate"] = [](const std::vector<std::string>& args) {
        return std::make_unique<GenerateCommand>(args);
    };

    registry_["print"] = [](const std::vector<std::string>& args) {
        return std::make_unique<PrintCommand>(args);
    };

    registry_["validate"] = [](const std::vector<std::string>& args) {
        return std::make_unique<ValidateCommand>(args);
    };

    registry_["help"] = [](const std::vector<std::string>& args) {
        return std::make_unique<HelpCommand>(args);
    };
}

std::unique_ptr<Command> CommandFactory::createCommand(const std::string& name,
                                                       const std::vector<std::string>& args) {
    auto it = registry_.find(name);
    if (it != registry_.end()) {
        return it->second(args);
    }
    return nullptr;
}

bool CommandFactory::isCommandRegistered(const std::string& name) const {
    return registry_.find(name) != registry_.end();
}

std::vector<std::string> CommandFactory::getAvailableCommands() const {
    std::vector<std::string> commands;
    commands.reserve(registry_.size());

    for (const auto& [name, _] : registry_) {
        commands.push_back(name);
    }

    std::sort(commands.begin(), commands.end());
    return commands;
}

}  // namespace app