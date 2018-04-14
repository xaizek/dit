// Copyright (C) 2016 xaizek <xaizek@posteo.net>
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

#include <cerrno>
#include <cstdlib>
#include <cstdio>

#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>

#include "utils/containers.hpp"
#include "utils/contains.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Config.hpp"
#include "Dit.hpp"
#include "Project.hpp"
#include "completion.hpp"

namespace fs = boost::filesystem;

namespace {

/**
 * @brief Implementation of "rename" command, which helps with completion.
 */
class RenameCmd : public AutoRegisteredCommand<RenameCmd>
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    RenameCmd();

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
};

}

RenameCmd::RenameCmd()
    : parent("rename", "rename a project",
             "Usage: rename <old name> <new name>")
{
}

boost::optional<int>
RenameCmd::run(Dit &dit, const std::vector<std::string> &args)
{
    if (args.size() != 2) {
        err() << "Expected two arguments (old and new project names).\n";
        return EXIT_FAILURE;
    }

    if (contains(args[0], '/')) {
        err() << "Project name can't contain slash: " << args[0] << '\n';
        return EXIT_FAILURE;
    }

    if (contains(args[1], '/')) {
        err() << "Project name can't contain slash: " << args[1] << '\n';
        return EXIT_FAILURE;
    }

    const fs::path projectsDir = dit.getProjectsDir();
    const std::string src = (projectsDir/args[0]).string();
    const std::string dst = (projectsDir/args[1]).string();

    if (!Project(src).exists()) {
        err() << "Source project not found.\n";
        return EXIT_FAILURE;
    }

    if (Project(dst).exists()) {
        err() << "Destination project already exists.\n";
        return EXIT_FAILURE;
    }

    if (std::rename(src.c_str(), dst.c_str()) != 0) {
        err() << strerror(errno) << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

boost::optional<int>
RenameCmd::complete(Dit &dit,
                    const std::vector<std::string> &args)
{
    if (args.size() > 2) {
        return EXIT_FAILURE;
    }

    for (const std::string &prj : sorted(listProjects(dit.getProjectsDir()))) {
        out() << prj << '\n';
    }

    return EXIT_SUCCESS;
}
