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
#include <boost/optional.hpp>

#include "Command.hpp"
#include "Commands.hpp"
#include "Config.hpp"
#include "Item.hpp"
#include "Project.hpp"

namespace fs = boost::filesystem;

/**
 * @brief Character in front of project name on command-line.
 */
const char PROJECT_PREFIX_CHAR = '.';

Scribe::Scribe(int argc, const char *const argv[])
{
    initArgs(argc, argv);
    initConfig();
}

Scribe::~Scribe()
{
}

void
Scribe::initArgs(int argc, const char *const argv[])
{
    if (argc < 1) {
        throw std::runtime_error("Broken argument list.");
    }

    appName = argv[0];

    // TODO: come up with better parsing code.
    int offset = 1;
    if (argc > 1 && argv[1][0] == PROJECT_PREFIX_CHAR) {
        prjName = &argv[1][1];
        ++offset;
    }
    std::copy(argv + offset, argv + argc, std::back_inserter(args));

    if (!args.empty()) {
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

    fs::path configHomePath;
    if (const char *const config_home_env = std::getenv("XDG_CONFIG_HOME")) {
        configHomePath = fs::path(config_home_env)/"scribe";
    } else {
        configHomePath = home/".config/scribe";
    }

    projectsDir = (configHomePath/"projects").string();

    std::string configPath = (configHomePath/"config").string();
    config.reset(new Config(configPath));
}

int
Scribe::run()
{
    if (cmdName.empty()) {
        cmdName = config->get("core.defcmd", "ls");
    }

    Command *const cmd = Commands::get(cmdName);
    if (cmd == nullptr) {
        std::cerr << "Unknown command name: " << cmdName << std::endl;
        return EXIT_FAILURE;
    }

    if (boost::optional<int> exitCode = cmd->run(*this, args)) {
        if (*exitCode == EXIT_SUCCESS) {
            config->save();
        }
        return *exitCode;
    }

    if (prjName.empty()) {
        prjName = config->get("core.defprj", "");
    }

    if (prjName.empty()) {
        std::cerr << "No project specified" << std::endl;
        return EXIT_FAILURE;
    }

    Project project((fs::path(projectsDir)/prjName).string());
    if (!project.exists()) {
        std::cerr << "Project does not exist: " << prjName << std::endl;
        return EXIT_FAILURE;
    }

    if (boost::optional<int> exitCode = cmd->run(project, args)) {
        if (*exitCode == EXIT_SUCCESS) {
            project.save();
            config->save();
        }

        return *exitCode;
    }

    assert(false && "Command has no or broken implementation.");
    return EXIT_FAILURE;
}

Config &
Scribe::getConfig()
{
    return *config;
}

const std::string &
Scribe::getProjectsDir() const
{
    return projectsDir;
}
