// Copyright (C) 2015 xaizek <xaizek@openmailbox.org>
//
// This file is part of scribe.
//
// scribe is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// scribe is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with scribe.  If not, see <http://www.gnu.org/licenses/>.

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

std::vector<std::reference_wrapper<Command>>
Commands::list()
{
    std::vector<std::reference_wrapper<Command>> list;
    list.reserve(cmds.size());
    for (auto &e : cmds) {
        list.emplace_back(*e.second.get());
    }

    return std::move(list);
}

Commands &
Commands::getThis()
{
    static Commands commands;
    return commands;
}
