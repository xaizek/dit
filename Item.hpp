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

#ifndef SCRIBE__ITEM_HPP__
#define SCRIBE__ITEM_HPP__

#include <set>
#include <string>
#include <vector>

#include "Change.hpp"

class Storage;

/**
 * @brief Single item to take care of.
 */
class Item
{
    friend class Storage;

private:
    /**
     * @brief Constructs new item.
     * @see Storage
     *
     * @param storage Storage this item belongs to.
     * @param id Id of the item.
     */
    Item(Storage &storage, std::string id);

public:
    /**
     * @brief Retrieves item id.
     *
     * @returns The id.
     */
    const std::string & getId() const;
    /**
     * @brief Retrieves current (latest) value for the key.
     *
     * @param key Key of the value.
     *
     * @returns The value or empty string if it doesn't exist.
     */
    std::string getValue(const std::string &key);
    /**
     * @brief Retrieves names of actually existing keys for this item.
     *
     * @returns The names.
     */
    std::set<std::string> listRecordNames();
    /**
     * @brief Changes @p key to have value of @p value.
     *
     * @p key is created if doesn't exist.
     *
     * @param key Key to set.
     * @param value (New) value for the @p key.
     */
    void setValue(const std::string &key, const std::string &value);
    /**
     * @brief Checks whether item has changes that require storing.
     *
     * @returns @c true if so, @c false otherwise.
     */
    bool wasChanged() const;

private:
    /**
     * @brief Ensures that data is loaded from storage.
     *
     * @throws std::runtime_error On missing/broken storage cell.
     */
    void ensureLoaded();
    /**
     * @brief Actually loads data from storage.
     *
     * @throws std::runtime_error On missing/broken storage cell.
     */
    void load();
    /**
     * @brief Retrieves latest change for the @p key.
     *
     * @param key Key to loop up for.
     *
     * @returns The latest change or @c nullptr if no change found.
     */
    Change * getLatestChange(const std::string &key);
    /**
     * @brief Retrieves latest change for the @p key before @p before.
     *
     * @param key Key to loop up for.
     * @param before Upper bound for the lookup.
     *
     * @returns The latest change or @c nullptr if no change found.
     */
    Change * getLatestChange(const std::string &key, Change *before);

private:
    /**
     * @brief Storage to which this item belongs.
     */
    Storage &storage;
    /**
     * @brief Id of this item.
     */
    const std::string id;
    /**
     * @brief Change set associated with the item.
     */
    std::vector<Change> changes;
    /**
     * @brief Whether item contains real data.
     *
     * Otherwise this instance represents an item and data will be loaded on
     * demand.
     */
    bool loaded;
    /**
     * @brief Whether item was changed.
     */
    bool changed;
};

#endif // SCRIBE__ITEM_HPP__
