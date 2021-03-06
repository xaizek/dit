// Copyright (C) 2015 xaizek <xaizek@posteo.net>
//
// This file is part of dit.
//
// dit is free software: you can redistribute it and/or modify
// it under the terms of version 3 of the GNU Affero General Public
// License as published by the Free Software Foundation.
//
// dit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dit.  If not, see <http://www.gnu.org/licenses/>.

#include "Commands.hpp"

#include <algorithm>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "utils/contains.hpp"
#include "Command.hpp"

/**
 * @brief Convenience macro for accessing @c commands field.
 */
#define cmds (getThis().commands)

void
Commands::add(std::unique_ptr<Command> cmd)
{
    const std::string cmdName = cmd->getName();

    if (contains(cmds, cmdName)) {
        throw std::runtime_error("Two commands named: " + cmdName);
    }

    cmds[cmdName] = std::move(cmd);
}

void
Commands::remove(const std::string &name, pk<Tests>)
{
    cmds.erase(name);
}

Command *
Commands::get(const std::string &name)
{
    auto it = cmds.find(name);
    return (it == cmds.cend()) ? nullptr : it->second.get();
}

std::vector<std::reference_wrapper<Command>>
Commands::list()
{
    std::vector<std::reference_wrapper<Command>> list;
    list.reserve(cmds.size());
    for (auto &e : cmds) {
        list.emplace_back(*e.second.get());
    }

    return list;
}

Commands &
Commands::getThis()
{
    static Commands commands;
    return commands;
}
