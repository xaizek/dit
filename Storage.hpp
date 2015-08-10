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

#ifndef SCRIBE__STORAGE_HPP__
#define SCRIBE__STORAGE_HPP__

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace boost { namespace filesystem {
  class path;
} }
namespace fs = boost::filesystem;

class Item;
class Project;

/**
 * @brief Storage for a set of items.
 */
class Storage
{
public:
    /**
     * @brief Creates storage for the @p project.
     *
     * @param project Project, which is parent of the storage.
     */
    Storage(Project &project);

public:
    /**
     * @brief Creates new item.
     *
     * @returns Reference to newly created item.
     */
    Item & create();
    /**
     * @brief Retrieves item by its id.
     *
     * @param id Id of item to retrieve.
     *
     * @returns Item.
     *
     * @throws std::runtime_error On unknown id.
     */
    Item & get(const std::string &id);
    /**
     * @brief Lists all available items.
     *
     * @returns Snapshot of current list of items.
     *
     * @throws boost::filesystem::filesystem_error On broken storage.
     */
    std::vector<std::reference_wrapper<Item>> list();
    /**
     * @brief Fills empty item with actual content.
     *
     * @param item Item to load data for.
     *
     * @throws std::runtime_error On missing item data.
     */
    void fill(Item &item);
    /**
     * @brief Stores changed items.
     *
     * @throws boost::filesystem::filesystem_error On issues with storage.
     * @throws std::runtime_error On data write failure.
     */
    void save();

private:
    /**
     * @brief Ensures that storage is loaded from physical source.
     *
     * @throws boost::filesystem::filesystem_error On broken storage.
     */
    void ensureLoaded();
    /**
     * @brief Actually loads storage from physical source, first level.
     *
     * @throws boost::filesystem::filesystem_error On broken storage.
     */
    void load();
    /**
     * @brief Loads items from a directory, second level.
     *
     * @param path Parent directories of items.
     *
     * @throws boost::filesystem::filesystem_error On broken storage.
     */
    void loadDir(const fs::path &path);

private:
    /**
     * @brief Project this storage belongs to.
     */
    Project &project;
    /**
     * @brief Items known to the storage.
     */
    std::map<std::string, Item> items;
    /**
     * @brief Whether storage is loaded from physical source.
     *
     * Otherwise it can contain newly created in-memory items, but be missing
     * items that were created and stored before.
     */
    bool loaded;
};

#endif // SCRIBE__STORAGE_HPP__
