// Copyright (C) 2015 xaizek <xaizek@openmailbox.org>
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

#include "Storage.hpp"

#include <cassert>

#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>

#include <boost/range/iterator_range.hpp>
#include <boost/filesystem.hpp>

#include "Change.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "file_format.hpp"

namespace fs = boost::filesystem;

void
Storage::init(Project &project)
{
    IdGenerator::init(project.getConfig(false),
                      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

Storage::Storage(Project &project)
    : project(project), idGenerator(project.getConfig(false))
{
}

Storage::Storage(Project &project, pk<Project>)
    : StorageBacked<Storage>(true), project(project),
      idGenerator(project.getConfig(false))
{
}

Item &
Storage::create()
{
    ensureLoaded();

    const std::string id = idGenerator.getId();

    decltype(items)::iterator it;
    bool inserted;
    std::tie(it, inserted) = items.emplace(id, Item(*this, id, false, {}));
    assert(inserted && "Duplicated item id");

    // Advance ID if we got here without exceptions.
    idGenerator.advanceId();

    return it->second;
}

void
Storage::put(Item item, pk<Tests>)
{
    bool inserted;
    std::tie(std::ignore, inserted) = items.emplace(item.getId(),
                                                    std::move(item));
    assert(inserted && "Duplicated item id");
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
Storage::load()
{
    const std::string &dataDir = project.getDataDir();

    // Suppress throwing exceptions if project doesn't have any items.
    if (project.exists() && !fs::is_directory(dataDir)) {
        return;
    }

    using dir_it = fs::directory_iterator;
    for (fs::directory_entry &e :
         boost::make_iterator_range(dir_it(dataDir), dir_it())) {
        loadDir(e.path());
    }
}

void
Storage::loadDir(const fs::path &path)
{
    using dir_it = fs::directory_iterator;
    fs::path prefix = path.filename();
    for (fs::directory_entry &e :
         boost::make_iterator_range(dir_it(path), dir_it())) {
        const std::string id = prefix.string() + e.path().filename().string();

        bool inserted = items.emplace(id, Item(*this, id, true, {})).second;
        assert(inserted && "Duplicated item id");
        (void)inserted;
    }
}

void
Storage::fill(Item &item, pk<Item>)
{
    const std::string &id = item.getId();
    const fs::path path =
        fs::path(project.getDataDir())/id.substr(0, 1)/id.substr(1);

    std::ifstream file(path.string());
    if (!file) {
        throw std::runtime_error("Failed to read change set of " + id);
    }

    file >> item.getChanges({});
}

void
Storage::save()
{
    for (const auto &e : items) {
        if (!e.second.wasChanged()) {
            continue;
        }

        const std::string &id = e.second.getId();

        const fs::path dirPath =
            fs::path(project.getDataDir())/id.substr(0, 1);

        if (!fs::exists(dirPath)) {
            fs::create_directories(dirPath);
        }

        const fs::path filePath = dirPath/id.substr(1);
        std::ofstream file(filePath.string());
        if (!file) {
            throw std::runtime_error("Failed to write change set of " +
                                     e.second.getId());
        }
        /* TODO: write only new changes (append them). */
        file << e.second.getChanges({});
    }

    idGenerator.save();
}
