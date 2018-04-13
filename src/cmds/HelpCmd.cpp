// Copyright (C) 2015 xaizek <xaizek@posteo.net>
//
// This file is part of dit.
//
// dit is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dit.  If not, see <http://www.gnu.org/licenses/>.

#include <cstdlib>

#include <ostream>
#include <string>
#include <vector>

#include "utils/containers.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "printing.hpp"

namespace {

/**
 * @brief Implementation of "help" command, which sets item information.
 */
class HelpCmd : public AutoRegisteredCommand<HelpCmd>
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
    virtual boost::optional<int> run(
        Dit &dit,
        const std::vector<std::string> &args) override;
    /**
     * @copydoc Command::complete()
     */
    virtual boost::optional<int> complete(
        Dit &dit,
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

}

HelpCmd::HelpCmd()
    : parent("help", "provide help information", "Usage: help [command]")
{
}

boost::optional<int>
HelpCmd::run(Dit &, const std::vector<std::string> &args)
{
    if (args.size() > 1U) {
        err() << "Expected at most one argument.\n";
        return EXIT_FAILURE;
    }

    if (args.empty()) {
        return listCommands();
    }

    const std::string &cmdName = args[0];
    return commandHelp(cmdName);
}

boost::optional<int>
HelpCmd::complete(Dit &, const std::vector<std::string> &args)
{
    if (args.size() > 1) {
        return EXIT_FAILURE;
    }

    for (Command &cmd : Commands::list()) {
        out() << cmd.getName() << '\n';
    }

    return EXIT_SUCCESS;
}

int
HelpCmd::listCommands()
{
    for (const Command &cmd : sorted(Commands::list())) {
        out() << Cmd{cmd.getName()} << " -- " << cmd.getDescr() << '\n';
    }

    return EXIT_SUCCESS;
}

int
HelpCmd::commandHelp(const std::string &cmdName)
{
    Command *const cmd = Commands::get(cmdName);
    if (cmd == nullptr) {
        err() << "Unknown command name: " << cmdName << '\n';
        return EXIT_FAILURE;
    }

    out() << Cmd{cmdName}
          << " -- "
          << cmd->getDescr() << "\n\n"
          << cmd->getHelp() << '\n';

    return EXIT_SUCCESS;
}
