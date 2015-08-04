#include "Commands.hpp"

#include <algorithm>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "Command.hpp"

/**
 * @brief Convenience macro for accessing @c commands field.
 */
#define cmds (getThis().commands)

void
Commands::add(std::unique_ptr<Command> cmd)
{
    const std::string cmdName = cmd->getName();

    if (cmds.find(cmdName) != cmds.end()) {
        throw std::runtime_error("Two commands named: " + cmdName);
    }

    cmds[cmdName] = std::move(cmd);
}

Command *
Commands::get(const std::string &name)
{
    return cmds[name].get();
}

std::vector<Command *>
Commands::list()
{
    std::vector<Command *> list;

    std::for_each(cmds.cbegin(), cmds.cend(),
        [&list](const std::pair<const std::string,
                                std::unique_ptr<Command>> &p) {
            list.push_back(p.second.get());
        });

    return std::move(list);
}

Commands &
Commands::getThis()
{
    static Commands commands;
    return commands;
}
