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

#ifndef DIT__COMMANDS_HPP__
#define DIT__COMMANDS_HPP__

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "utils/Passkey.hpp"
#include "Command.hpp"

class Tests;

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
     * @see AutoRegisteredCommand
     *
     * @param cmd An instance of the command.
     *
     * @throws std::runtime_error On duplicated command name.
     */
    static void add(std::unique_ptr<Command> cmd);
    /**
     * @brief Removes command by its name.
     *
     * @param name Name of command to remove.
     */
    static void remove(const std::string &name, pk<Tests>);
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
 * @brief Helper class that auto-registers its derivative in Commands-list.
 *
 * @tparam C Derived class (as per CRTP).
 */
template <class C>
class AutoRegisteredCommand : public Command
{
public:
    /**
     * @brief Pull in parent constructor.
     */
    using Command::Command;

protected:
    /**
     * @brief Handy typedef for derived classes to ease parent construction.
     */
    using parent = AutoRegisteredCommand;

private:
    /**
     * @brief Static initialization of this variable performs the registration.
     */
    static const bool invokeRegister;

private:
    /**
     * @brief Purpose of this field it to make @c invokeRegister used.
     */
    const bool forceRegistration = invokeRegister;
};

template <class C>
const bool AutoRegisteredCommand<C>::invokeRegister = []() {
    Commands::add(std::unique_ptr<Command>(new C()));
    return true;
}();

#endif // DIT__COMMANDS_HPP__
