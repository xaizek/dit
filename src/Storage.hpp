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

#include "utils/Passkey.hpp"
#include "IdGenerator.hpp"
#include "LazyLoadable.hpp"

namespace boost { namespace filesystem {
    class path;
} }

class Item;
class Project;
class Tests;

/**
 * @brief Storage for a set of items.
 */
class Storage : private LazyLoadable<Storage>
{
    friend class LazyLoadable<Storage>;

public:
    /**
     * @brief Initializes storage for a new project.
     *
     * @param project Project to initialize.
     */
    static void init(Project &project);

public:
    /**
     * @brief Creates storage for the @p project.
     *
     * @param project Project, which is parent of the storage.
     */
    Storage(Project &project);
    /**
     * @brief Creates storage for the @p project, which is marked as loaded.
     *
     * @param project Project, which is parent of the storage.
     */
    Storage(Project &project, pk<Project>);
    /**
     * @brief Move constructor.
     *
     * @param rhs Object to steal data from.
     */
    Storage(Storage &&rhs);

public:
    /**
     * @brief Creates new item.
     *
     * @returns Reference to newly created item.
     */
    Item & create();
    /**
     * @brief Inserts the item into the storage.
     *
     * @param item Item to insert.
     */
    void put(Item item, pk<Tests>);
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
    void fill(Item &item, pk<Item>);
    /**
     * @brief Stores changed items.
     *
     * @throws boost::filesystem::filesystem_error On issues with storage.
     * @throws std::runtime_error On data write failure.
     */
    void save();

    /**
     * @brief Updates stored link to parent project.
     *
     * A helper for fixing up storage after a move.
     *
     * @param project New project object location.
     */
    void relinkProject(Project &project);

private:
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
    void loadDir(const boost::filesystem::path &path);

private:
    /**
     * @brief Project this storage belongs to.
     */
    std::reference_wrapper<Project> project;
    /**
     * @brief Items known to the storage.
     */
    std::map<std::string, Item> items;
    /**
     * @brief Implementation of ID generation algorithm.
     */
    IdGenerator idGenerator;
};

#endif // SCRIBE__STORAGE_HPP__
