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

#include <cstdlib>

#include <algorithm>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <boost/range/iterator_range.hpp>
#include <boost/filesystem.hpp>

#include "Command.hpp"
#include "Commands.hpp"
#include "Config.hpp"
#include "Dit.hpp"
#include "Project.hpp"
#include "decoration.hpp"

namespace fs = boost::filesystem;

/**
 * @brief Usage message for "projects" command.
 */
const char *const USAGE = R"(Usage: projects

Prints list like the following:
 <project-name>
 <project-name> -- <description ("prj.descr")>
*<project-name>

Where <description> is optional and "*" signifies active project (either default
one or explicitly specified on the command-line.)";

namespace {

/**
 * @brief Implementation of "projects" command, which lists projects.
 */
class ProjectsCmd : public AutoRegisteredCommand<ProjectsCmd>
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    ProjectsCmd();

public:
    /**
     * @copydoc Command::run()
     */
    virtual boost::optional<int> run(
        Dit &dit,
        const std::vector<std::string> &args) override;
};

}

ProjectsCmd::ProjectsCmd() : parent("projects", "list projects", USAGE)
{
}

boost::optional<int>
ProjectsCmd::run(Dit &dit, const std::vector<std::string> &args)
{
    if (!args.empty()) {
        err() << "Expected no arguments.\n";
        return EXIT_FAILURE;
    }

    const std::string &projectsDir = dit.getProjectsDir();
    if (!fs::exists(projectsDir)) {
        return EXIT_SUCCESS;
    }

    std::vector<std::pair<std::string, std::string>> infos;

    // Gather project information.
    using dir_it = fs::directory_iterator;
    for (fs::directory_entry &e :
         boost::make_iterator_range(dir_it(projectsDir), dir_it())) {
        Project prj(e.path().string());
        if (!prj.exists()) {
            continue;
        }

        const std::string &descr = prj.getConfig().get("prj.descr", "");
        infos.emplace_back(prj.getName(), descr);
    }

    // Sort them.
    std::sort(infos.begin(), infos.end());

    // Display.
    const std::string &prj = dit.getPrj();
    for (const std::pair<std::string, std::string> &info : infos) {
        out() << (decor::bold << (info.first == prj ? '*' : ' ') << info.first);
        if (!info.second.empty()) {
            out() << " -- " << info.second;
        }
        out() << '\n';
    }

    return EXIT_SUCCESS;
}
