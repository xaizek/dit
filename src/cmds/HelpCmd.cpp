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

#include <ostream>
#include <string>
#include <vector>

#include "utils/containers.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "decoration.hpp"

static bool operator<(const Command &l, const Command &r);

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
        Scribe &scribe,
        const std::vector<std::string> &args) override;
    /**
     * @copydoc Command::complete()
     */
    virtual boost::optional<int> complete(
        Scribe &scribe,
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
    : parent("help", "help information", "Usage: help [command]")
{
}

boost::optional<int>
HelpCmd::run(Scribe &, const std::vector<std::string> &args)
{
    if (args.size() > 1) {
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
HelpCmd::complete(Scribe &, const std::vector<std::string> &args)
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
    for (Command &cmd : sorted(Commands::list())) {
        out() << decor::bold << cmd.getName() << decor::def
              << " -- "
              << cmd.getDescr() << '\n';
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Compares two commands by their names.
 *
 * @param l First command to compare.
 * @param r Second command to compare.
 *
 * @returns @c true if @p l is "less than" @p r, otherwise @c false is returned.
 */
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
        err() << "Unknown command name: " << cmdName << '\n';
        return EXIT_FAILURE;
    }

    out() << decor::bold << cmdName << decor::def
          << " -- "
          << cmd->getDescr() << "\n\n"
          << cmd->getHelp() << '\n';

    return EXIT_SUCCESS;
}
