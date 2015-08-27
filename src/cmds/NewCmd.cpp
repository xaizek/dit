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

#include <boost/filesystem.hpp>

#include "Command.hpp"
#include "Commands.hpp"
#include "Project.hpp"
#include "Scribe.hpp"

namespace fs = boost::filesystem;

namespace {

/**
 * @brief Implementation of "new" command, which creates projects.
 */
class NewCmd : public Command
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    NewCmd();

public:
    /**
     * @copydoc Command::run()
     */
    virtual boost::optional<int> run(
        Scribe &scribe,
        const std::vector<std::string> &args) override;
};

REGISTER_COMMAND(NewCmd);

}

NewCmd::NewCmd() : Command("new", "creates projects", "Usage: new project-name")
{
}

boost::optional<int>
NewCmd::run(Scribe &scribe, const std::vector<std::string> &args)
{
    if (args.size() != 1) {
        err() << "Expected single argument (project name).\n";
        return EXIT_FAILURE;
    }

    const std::string &prjName = args[0];
    std::string rootDir = (fs::path(scribe.getProjectsDir())/prjName).string();

    if (Project(rootDir).exists()) {
        err() << "project already exists: " << prjName << '\n';
        return EXIT_FAILURE;
    }

    Project::init(rootDir);

    return EXIT_SUCCESS;
}
