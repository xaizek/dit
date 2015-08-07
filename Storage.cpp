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

#include "Storage.hpp"

#include <cassert>

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <boost/filesystem.hpp>

#include "Item.hpp"
#include "Project.hpp"
#include "file_format.hpp"

namespace fs = boost::filesystem;

Storage::Storage(Project &project) : project(project), loaded(false)
{
}

Item &
Storage::create()
{
    decltype(items)::iterator it;
    bool inserted;
    std::tie(it, inserted) = items.emplace("fid", Item(*this, "fid"));
    assert(inserted && "Duplicated item id");

    return it->second;
}

Item &
Storage::get(const std::string &id)
{
    ensureLoaded();

    const decltype(items)::iterator it = items.find(id);
    if (it == items.end()) {
        throw std::runtime_error("Unknown id: " + id);
    }

    return it->second;
}

std::vector<std::reference_wrapper<Item>>
Storage::list()
{
    ensureLoaded();

    std::vector<std::reference_wrapper<Item>> list;
    list.reserve(items.size());
    for (auto &e : items) {
        list.emplace_back(e.second);
    }
    return list;
}

void
Storage::ensureLoaded()
{
    if (!loaded) {
        load();
        loaded = true;
    }
}

void
Storage::load()
{
    const std::string &dataDir = project.getDataDir();
    for (fs::directory_entry &e : fs::directory_iterator(dataDir)) {
        loadDir(e.path());
    }
}

void
Storage::loadDir(const fs::path &path)
{
    fs::path prefix = path.filename();
    for (fs::directory_entry &e : fs::directory_iterator(path)) {
        const std::string id = prefix.string() + e.path().filename().string();

        bool inserted;
        std::tie(std::ignore, inserted) = items.emplace(id, Item(*this, id));
        assert(inserted && "Duplicated item id");
    }
}

void
Storage::fill(Item &item)
{
    const std::string &id = item.getId();
    const fs::path path =
        fs::path(project.getDataDir())/id.substr(0, 1)/id.substr(1);

    std::ifstream file(path.string());
    if (!file) {
        throw std::runtime_error("Failed to read change set of " + id);
    }

    file >> item.changes;
}
