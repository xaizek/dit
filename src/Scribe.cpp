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
#include <vector>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "utils/containers.hpp"
#include "utils/memory.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Config.hpp"
#include "Invocation.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "parsing.hpp"

namespace fs = boost::filesystem;

static Config & getDefaultConfig();
static std::vector<std::string> listProjects(const std::string &projectsDir);
static std::vector<std::string> listCommands(Config &config);

/**
 * @brief Mark of a cursor position during completion.
 */
const std::string COMPL_CURSOR_MARK = "::cursor::";

Scribe::Scribe(std::vector<std::string> args)
{
    if (args.empty()) {
        throw std::runtime_error("Broken argument list.");
    }

    initConfig();

    invocation.setCmdLine({ args.begin() + 1, args.end() });
    invocation.setDefCmdLine(globalConfig->get("core.defcmd"));
    invocation.setDefPrjName(globalConfig->get("core.defprj"));
    invocation.setAliasResolver([this](const std::string &name) {
        return globalConfig->get("alias." + name, std::string());
    });
}

Scribe::~Scribe()
{
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
    globalConfig.reset(new Config(configPath, &getDefaultConfig()));
}

/**
 * @brief Retrieves default configuration.
 *
 * @returns Configuration filled by global defaults.
 */
static Config &
getDefaultConfig()
{
    static Config defCfg((std::string()));
    static bool inited;
    if (!inited) {
        inited = true;

        defCfg.set("core.defcmd", "ls");
        defCfg.set("core.defprj", std::string());
        defCfg.set("ui.ls.fmt", "_id,title");
        defCfg.set("ui.ls.sort", "title,_id");
        defCfg.set("ui.ls.color", "fg-cyan inv bold !heading");
        defCfg.set("ui.show.order", "title");
    }

    return defCfg;
}

int
Scribe::run()
{
    invocation.parse();
    const std::string cmdName = invocation.getCmdName();
    const std::vector<std::string> cmdArgs = invocation.getCmdArgs();

    Command *const cmd = Commands::get(cmdName);
    if (cmd == nullptr) {
        std::cerr << "Unknown command name: " << cmdName << std::endl;
        return EXIT_FAILURE;
    }

    if (boost::optional<int> exitCode = cmd->run(*this, cmdArgs)) {
        if (*exitCode == EXIT_SUCCESS) {
            globalConfig->save();
        }
        return *exitCode;
    }

    const std::string prjName = invocation.getPrjName();
    std::string error;
    std::unique_ptr<Project> prj = openProject(prjName, error);
    if (!prj) {
        std::cerr << error << std::endl;
        return EXIT_FAILURE;
    }

    if (boost::optional<int> exitCode = cmd->run(*prj, cmdArgs)) {
        if (*exitCode == EXIT_SUCCESS) {
            prj->save();
            globalConfig->save();
        }

        return *exitCode;
    }

    assert(false && "Command has no or broken implementation.");
    return EXIT_FAILURE;
}

int
Scribe::complete(std::vector<std::string> args, std::ostream &out,
                 std::ostream &)
{
    invocation.setCmdLine(std::move(args));
    invocation.parse(true);

    std::vector<std::string> names;

    std::string composition = invocation.getComposition();

    if (boost::ends_with(invocation.getPrjName(), COMPL_CURSOR_MARK)) {
        names = listProjects(projectsDir);
    } else if (boost::ends_with(composition, COMPL_CURSOR_MARK)) {
        names = listCommands(*globalConfig);
        if (composition.find('.') != std::string::npos) {
            const std::string prefix =
                composition.substr(0, composition.rfind('.') + 1);
            for (std::string &name : names) {
                name = prefix + name;
            }
        }
    } else {
        return completeCmd();
    }

    for (const std::string &name : sorted(std::move(names))) {
        out << name << '\n';
    }

    return EXIT_SUCCESS;
}

int
Scribe::completeCmd()
{
    const std::string prjName = invocation.getPrjName();
    std::string error;
    std::unique_ptr<Project> prj = openProject(prjName, error);
    if (!prj) {
        std::cerr << error << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<std::string> args = invocation.getCmdArgs();
    if (!args.empty()) {
        std::string &last = args.back();
        last = last.substr(0, last.length() - COMPL_CURSOR_MARK.length());
    }

    Command *const cmd = Commands::get(invocation.getCmdName());
    if (cmd == nullptr) {
        return EXIT_FAILURE;
    }

    if (boost::optional<int> exitCode = cmd->complete(*this, args)) {
        return *exitCode;
    }
    if (boost::optional<int> exitCode = cmd->complete(*prj, args)) {
        return *exitCode;
    }

    return EXIT_FAILURE;
}


/**
 * @brief Lists all projects.
 *
 * @param projectsDir Base directory for projects.
 *
 * @returns Names of discovered projects with prepended dot (.).
 */
static std::vector<std::string>
listProjects(const std::string &projectsDir)
{
    std::vector<std::string> names;

    for (fs::directory_entry &e : fs::directory_iterator(projectsDir)) {
        if (Project(e.path().string()).exists()) {
            names.push_back('.' + e.path().filename().string());
        }
    }

    return std::move(names);
}

/**
 * @brief Lists all commands.
 *
 * @param config Configuration to get aliases from.
 *
 * @returns Names of discovered commands.
 */
static std::vector<std::string>
listCommands(Config &config)
{
    std::vector<std::string> names;

    for (Command &cmd : Commands::list()) {
        names.push_back(cmd.getName());
    }

    for (const std::string &alias : config.list("alias")) {
        names.push_back(alias);
    }

    return std::move(names);
}

std::unique_ptr<Project>
Scribe::openProject(const std::string &name, std::string &error)
{
    if (name.empty()) {
        error = "No project specified";
        return {};
    }

    auto makeConfig = std::bind(std::mem_fn(&Scribe::makeConfig), this,
                                std::placeholders::_1);
    auto project = make_unique<Project>((fs::path(projectsDir)/name).string(),
                                        makeConfig);
    if (!project->exists()) {
        error = "Project does not exist: " + name;
        return {};
    }

    return std::move(project);
}

std::pair<Config, std::unique_ptr<Config>>
Scribe::makeConfig(const std::string &path) const
{
    auto prjCfg = make_unique<Config>(path, globalConfig.get());
    Config cfgProxy(std::string(), prjCfg.get());

    using confType = std::pair<std::string, std::string>;
    for (const confType &conf : invocation.getConfs()) {
        std::string key = conf.first;
        std::string value = conf.second;

        if (!key.empty() && key.back() == '+') {
            key.pop_back();
            value = cfgProxy.get(key, std::string()) + value;
        }
        cfgProxy.set(key, value);
    }

    return { std::move(cfgProxy), std::move(prjCfg) };
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

std::string
Scribe::getPrj() const
{
    return invocation.getPrjName();
}
