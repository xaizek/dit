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

#include "Project.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/filesystem.hpp>

#include "utils/memory.hpp"
#include "Config.hpp"
#include "Item.hpp"
#include "Storage.hpp"

namespace fs = boost::filesystem;

static std::pair<Config, std::unique_ptr<Config>> makeConfig(std::string path);
static std::string getSubRootPath(const fs::path &rootPath,
                                  const std::string &subPath);

void
Project::init(const std::string &rootDir)
{
    if (!fs::create_directories(rootDir)) {
        throw std::runtime_error("Directory already exists at: " + rootDir);
    }

    Project prj(rootDir);
    Storage::init(prj);
    prj.save();
}

Project::Project(std::string rootDir)
    : Project(std::move(rootDir), &makeConfig)
{
    dataDir = getSubRootPath(this->rootDir, "items");
}

Project::Project(std::string rootDir, mkConfig makeConfig)
    : storage(*this),
      configs(makeConfig(getSubRootPath(rootDir, "config"))),
      rootDir(std::move(rootDir))
{
    dataDir = getSubRootPath(this->rootDir, "items");
}

Project::Project(std::string rootDir, mkConfig makeConfig, pk<Tests>)
    : storage(*this, {}),
      configs((makeConfig ? makeConfig : &::makeConfig)
              (getSubRootPath(rootDir, "config"))),
      rootDir(std::move(rootDir))
{
    dataDir = getSubRootPath(this->rootDir, "items");
}

/**
 * @brief Makes standalone configuration for a project.
 *
 * @param path Path to real configuration file.
 *
 * @returns Pair of proxy (unsaved) and real configuration.
 */
static std::pair<Config, std::unique_ptr<Config>>
makeConfig(std::string path)
{
    auto prjCfg = make_unique<Config>(path);
    Config cfgProxy("<proxy>", prjCfg.get());
    return { std::move(cfgProxy), std::move(prjCfg) };
}

Project::Project(Project &&rhs)
    : storage(std::move(rhs.storage)), configs(std::move(rhs.configs)),
      rootDir(std::move(rhs.rootDir)), dataDir(std::move(rhs.dataDir))
{
    // Storage saves pointer to project, which must be updated after a move.
    storage.relinkProject(*this);
}

Project::~Project()
{
}

/**
 * @brief Retrieves path to configuration file of the project.
 *
 * @returns The path.
 */
static std::string
getSubRootPath(const fs::path &rootPath, const std::string &subPath)
{
    return (rootPath/subPath).string();
}

bool
Project::exists() const
{
    return fs::is_directory(rootDir);
}

std::string
Project::getName() const
{
    return fs::path(rootDir).filename().string();
}

Storage &
Project::getStorage()
{
    return storage;
}

Config &
Project::getConfig(bool proxy)
{
    return proxy ? configs.first : *configs.second;
}

const std::string &
Project::getDataDir() const
{
    return dataDir;
}

void
Project::save()
{
    // Since storage uses config, order here matters.
    storage.save();
    configs.second->save();
}
