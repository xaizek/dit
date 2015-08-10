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

#ifndef SCRIBE__COMMANDS_HPP__
#define SCRIBE__COMMANDS_HPP__

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Command;

/**
 * @brief Keeps track of all sub-commands.
 */
class Commands
{
private:
    /**
     * @brief This class is not ment to be instantiated from the outside.
     */
    Commands() = default;

public:
    /**
     * @brief Registers command.
     * @see REGISTER_COMMAND
     *
     * @param cmd An instance of the command.
     *
     * @throws std::runtime_error On duplicated command name.
     */
    static void add(std::unique_ptr<Command> cmd);
    /**
     * @brief Retrieves command by its name.
     *
     * @param name Name of command to look up for.
     *
     * @returns The command or @c nullptr if command not found.
     */
    static Command * get(const std::string &name);
    /**
     * @brief Lists all available commands.
     *
     * @returns Vector of all commands.
     */
    static std::vector<std::reference_wrapper<Command>> list();

private:
    /**
     * @brief Provides access to Commands instance.
     *
     * @returns The instance.
     */
    static Commands & getThis();

private:
    /**
     * @brief All available commands, registered via @c add().
     */
    std::map<std::string, std::unique_ptr<Command>> commands;
};

/**
 * @brief Helper macro that auto-registers Command-class in Commands-list.
 *
 * @param className Name of the class derived from @c Command.
 */
#define REGISTER_COMMAND(className) \
    static int \
    doReg ## className() \
    { \
        Commands::add(std::unique_ptr<Command>(new className())); \
        return 0; \
    } \
    const int reg ## className = doReg ## className();

#endif // SCRIBE__COMMANDS_HPP__
