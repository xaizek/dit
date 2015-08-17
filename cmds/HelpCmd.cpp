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

#include <cstdlib>

#include <iostream>
#include <string>
#include <vector>

#include "utils/containers.hpp"
#include "Command.hpp"
#include "Commands.hpp"

static bool operator<(const Command &l, const Command &r);

namespace {

/**
 * @brief Implementation of "help" command, which sets item information.
 */
class HelpCmd : public Command
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    HelpCmd();

public:
    /**
     * @copydoc Command::run()
     */
    virtual int run(Project &project,
                    const std::vector<std::string> &args) override;

private:
    /**
     * @brief Lists all commands with short description.
     *
     * @returns Exit code to be returned by run.
     */
    int listCommands();
    /**
     * @brief Displays help message for the specific command.
     *
     * @param cmdName Name of the command to display help about.
     *
     * @returns Exit code to be returned by run.
     */
    int commandHelp(const std::string &cmdName);
};

REGISTER_COMMAND(HelpCmd);

}

HelpCmd::HelpCmd()
    : Command("help", "help information", "Usage: help [command]")
{
}

int
HelpCmd::run(Project &, const std::vector<std::string> &args)
{
    if (args.size() > 1) {
        std::cerr << "help: Expected at most one argument." << std::endl;
        return EXIT_FAILURE;
    }

    if (args.empty()) {
        return listCommands();
    }

    const std::string &cmdName = args[0];
    return commandHelp(cmdName);
}

int
HelpCmd::listCommands()
{
    for (Command &cmd : sorted(Commands::list())) {
        std::cout << cmd.getName() << " -- " << cmd.getDescr() << '\n';
    }
    std::cout.flush();

    return EXIT_SUCCESS;
}

static bool
operator<(const Command &l, const Command &r)
{
    return l.getName() < r.getName();
}

int
HelpCmd::commandHelp(const std::string &cmdName)
{
    Command *const cmd = Commands::get(cmdName);
    if (cmd == nullptr) {
        std::cerr << "help: Unknown command name: " << cmdName << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << cmdName << " -- " << cmd->getDescr() << '\n'
              << cmd->getHelp() << std::endl;

    return EXIT_SUCCESS;
}