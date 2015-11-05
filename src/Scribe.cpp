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
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "utils/containers.hpp"
#include "utils/memory.hpp"
#include "utils/strings.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Config.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "parsing.hpp"

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

    // TODO: come up with better parsing code.
    int offset = 1;
    if (argc > 1 && argv[1][0] == PROJECT_PREFIX_CHAR) {
        prjName = &argv[1][1];
        ++offset;
    }

    std::vector<std::string> assigns;
    std::tie(assigns, args) = span(std::vector<std::string>(argv + offset,
                                                            argv + argc),
                                   [](const std::string &s){
                                      return s.find('=') != std::string::npos;
                                   });

    confs.reserve(assigns.size());
    for (const std::string &assign : assigns) {
        confs.emplace_back(splitAt(assign, '='));
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
    globalConfig.reset(new Config(configPath));
}

int
Scribe::run()
{
    if (prjName.empty()) {
        prjName = globalConfig->get("core.defprj", "");
    }

    if (args.empty()) {
        args = breakIntoArgs(globalConfig->get("core.defcmd", "ls"));
    }
    auto aliasResolver = std::bind(std::mem_fn(&Scribe::resolveAlias), this,
                                   std::placeholders::_1);
    std::string cmdName = parseInvocation(args, aliasResolver, false);

    Command *const cmd = Commands::get(cmdName);
    if (cmd == nullptr) {
        std::cerr << "Unknown command name: " << cmdName << std::endl;
        return EXIT_FAILURE;
    }

    if (boost::optional<int> exitCode = cmd->run(*this, args)) {
        if (*exitCode == EXIT_SUCCESS) {
            globalConfig->save();
        }
        return *exitCode;
    }

    if (prjName.empty()) {
        std::cerr << "No project specified" << std::endl;
        return EXIT_FAILURE;
    }

    auto makeConfig = std::bind(std::mem_fn(&Scribe::makeConfig), this,
                                std::placeholders::_1);
    Project project((fs::path(projectsDir)/prjName).string(), makeConfig);
    if (!project.exists()) {
        std::cerr << "Project does not exist: " << prjName << std::endl;
        return EXIT_FAILURE;
    }

    if (boost::optional<int> exitCode = cmd->run(project, args)) {
        if (*exitCode == EXIT_SUCCESS) {
            project.save();
            globalConfig->save();
        }

        return *exitCode;
    }

    assert(false && "Command has no or broken implementation.");
    return EXIT_FAILURE;
}

std::pair<Config, std::unique_ptr<Config>>
Scribe::makeConfig(const std::string &path) const
{
    auto prjCfg = make_unique<Config>(path, globalConfig.get());
    Config cfgProxy(std::string(), prjCfg.get());

    for (const std::pair<std::string, std::string> &conf : confs) {
        cfgProxy.set(conf.first, conf.second);
    }

    return { std::move(cfgProxy), std::move(prjCfg) };
}

int
Scribe::complete(Project &project, std::vector<std::string> args)
{
    auto aliasResolver = std::bind(std::mem_fn(&Scribe::resolveAlias), this,
                                   std::placeholders::_1);
    const std::string &cmdName = parseInvocation(args, aliasResolver, true);

    Command *const cmd = Commands::get(cmdName);
    if (cmd == nullptr) {
        return EXIT_FAILURE;
    }

    if (boost::optional<int> exitCode = cmd->complete(*this, args)) {
        return *exitCode;
    }
    if (boost::optional<int> exitCode = cmd->complete(project, args)) {
        return *exitCode;
    }

    return EXIT_FAILURE;
}

std::string
Scribe::resolveAlias(const std::string &name) const
{
    return globalConfig->get("alias." + name, std::string());
}

Config &
Scribe::getConfig()
{
    return *globalConfig;
}

const std::string &
Scribe::getProjectsDir() const
{
    return projectsDir;
}

const std::string &
Scribe::getPrj() const
{
    return prjName;
}
