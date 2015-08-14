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

#include <string>
#include <utility>

#include <boost/filesystem.hpp>

#include "Config.hpp"
#include "Item.hpp"
#include "Storage.hpp"

namespace fs = boost::filesystem;

Project::Project(std::string rootDir)
    : storage(*this), config(*this), rootDir(std::move(rootDir))
{
    dataDir = (fs::path(this->rootDir)/"items").string();
}

Storage &
Project::getStorage()
{
    return storage;
}

Config &
Project::getConfig()
{
    return config;
}

const std::string &
Project::getRootDir() const
{
    return rootDir;
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
    config.save();
}
