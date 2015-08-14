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

#include "Scribe.hpp"

#include <cassert>
#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/filesystem.hpp>

#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"

namespace fs = boost::filesystem;

/**
 * @brief Default command name used when run without arguments.
 */
static std::string DEFAULT_CMD = "ls";

Scribe::Scribe(int argc, const char *const argv[])
{
    initArgs(argc, argv);
    initConfig();
}

void
Scribe::initArgs(int argc, const char *const argv[])
{
    if (argc < 1) {
        throw std::runtime_error("Broken argument list.");
    }

    appName = argv[0];
    std::copy(argv + 1, argv + argc, std::back_inserter(args));

    if (args.empty()) {
        cmdName = DEFAULT_CMD;
    } else {
        cmdName = args[0];
        args.erase(args.begin());
    }
}

void
Scribe::initConfig()
{
    fs::path home;
    if (const char *const home_env = std::getenv("HOME")) {
        home = home_env;
    } else {
        throw std::runtime_error("HOME environment variable is not set.");
    }

    fs::path config_home;
    if (const char *const config_home_env = std::getenv("XDG_CONFIG_HOME")) {
        config_home = fs::path(config_home_env)/"scribe";
    } else {
        config_home = home/".config/scribe";
    }

    projectsDir = (config_home/"projects").string();
}

int
Scribe::run()
{
    Command *const cmd = Commands::get(cmdName);
    if (cmd == nullptr) {
        std::cerr << "Unknown command name: " << cmdName << std::endl;
        return EXIT_FAILURE;
    }

    Project project((fs::path(projectsDir)/"app").string());
    cmd->run(project, args);

    project.save();

    return EXIT_SUCCESS;
}
