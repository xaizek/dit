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

#include <cassert>
#include <cstdlib>

#include <functional>
#include <ostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "Command.hpp"
#include "Commands.hpp"
#include "Config.hpp"
#include "Project.hpp"
#include "Scribe.hpp"

namespace fs = boost::filesystem;

/**
 * @brief Usage message for "ls" command.
 */
const char *const USAGE = R"(Usage: complete what

Where <what> is one of:

    args ...  --  last argument of command line for a command
    commands  --  list of commands and aliases
    projects  --  list of available projects

Output is not sorted.)";

namespace {

/**
 * @brief Implementation of "complete" command, which helps with completion.
 */
class CompleteCmd : public Command
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    CompleteCmd();

public:
    /**
     * @copydoc Command::run()
     */
    virtual boost::optional<int> run(
        Scribe &scribe,
        const std::vector<std::string> &args) override;
    /**
     * @copydoc Command::run()
     */
    virtual boost::optional<int> run(
        Project &project,
        const std::vector<std::string> &args) override;
    /**
     * @copydoc Command::complete()
     */
    virtual boost::optional<int> complete(
        Scribe &scribe,
        const std::vector<std::string> &args) override;

private:
    /**
     * @brief Lists all projects.
     *
     * @param projectsDir Base directory for projects.
     *
     * @returns Exit code to be returned by run.
     */
    int listProjects(const std::string &projectsDir);
    /**
     * @brief Lists all commands.
     *
     * @param config Configuration to get aliases from.
     *
     * @returns Exit code to be returned by run.
     */
    int listCommands(Config &config);

private:
    /**
     * @brief Storage for the pointer between calls of @c run().
     */
    Scribe *scribe;
};

REGISTER_COMMAND(CompleteCmd);

}

CompleteCmd::CompleteCmd()
    : Command("complete", "provides completion lists", USAGE), scribe(nullptr)
{
}

boost::optional<int>
CompleteCmd::run(Scribe &scribe, const std::vector<std::string> &args)
{
    this->scribe = &scribe;

    if (args.size() == 0 || (args.size() > 1 && args[0] != "args")) {
        err() << "Wrong number of arguments.\n";
        return EXIT_FAILURE;
    }

    const std::string &what = args[0];

    if (what == "args") {
        return {};
    } else if (what == "commands") {
        return listCommands(scribe.getConfig());
    } else if (what == "projects") {
        return listProjects(scribe.getProjectsDir());
    } else {
        err() << "Unknown argument: " << what << '\n';
        return EXIT_FAILURE;
    }
}

boost::optional<int>
CompleteCmd::run(Project &project, const std::vector<std::string> &args)
{
    assert(args[0] == "args" && "Incorrect arguments for this overload.");

    return scribe->complete(project, std::vector<std::string>(args.begin() + 1,
                                                              args.end()));
}

boost::optional<int>
CompleteCmd::complete(Scribe &, const std::vector<std::string> &args)
{
    if (args.size() > 1) {
        return EXIT_FAILURE;
    }

    out() << "args\n";
    out() << "commands\n";
    out() << "projects\n";

    return EXIT_SUCCESS;
}

int
CompleteCmd::listProjects(const std::string &projectsDir)
{
    for (fs::directory_entry &e : fs::directory_iterator(projectsDir)) {
        if (Project(e.path().string()).exists()) {
            out() << '.' << e.path().filename().string() << '\n';
        }
    }
    return EXIT_SUCCESS;
}

int
CompleteCmd::listCommands(Config &config)
{
    for (Command &cmd : Commands::list()) {
        out() << cmd.getName() << '\n';
    }

    for (const std::string &alias : config.list("alias")) {
        out() << alias << '\n';
    }

    return EXIT_SUCCESS;
}
