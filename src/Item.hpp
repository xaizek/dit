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

#include <ctime>

#include <functional>
#include <set>
#include <string>
#include <vector>

#include "utils/Passkey.hpp"
#include "LazyLoadable.hpp"

class Change;
class Storage;
class Tests;

/**
 * @brief Single item to take care of.
 *
 * Pseudo-keys start with '_', such keys are read-only.
 * Keys can include only [-a-zA-Z_0-9], but can't start with [-_0-9].
 */
class Item : private LazyLoadable<Item>
{
    friend class LazyLoadable<Item>;

public:
    /**
     * @brief Checks whether key name is of valid format.
     *
     * @param name Key name to evaluate.
     * @param forWrite Whether key is going to be changed.
     * @param[out] error Additional error message on wrong keys.
     *
     * @returns @c true if key name is valid, @c false otherwise.
     */
    static bool isValidKeyName(const std::string &name, bool forWrite,
                               std::string &error);

public:
    /**
     * @brief Constructs new item.
     * @see Storage
     *
     * @param storage Storage this item belongs to.
     * @param id Id of the item.
     * @param exists Whether this item is a dummy until loaded.
     */
    Item(Storage &storage, std::string id, bool exists, pk<Storage>);
    /**
     * @brief Constructs new item for testing purposes.
     *
     * @param id Id of the item.
     */
    Item(std::string id, pk<Tests>);

public:
    /**
     * @brief Copy constructor is not available.
     *
     * The reason is that Items lifetime is managed by Storage and making copies
     * would change that.
     *
     * @param rhs Copy source.
     */
    Item(const Item &rhs) = delete;
    /**
     * @brief Move constructor is available.
     *
     * This one has to be public to make type moving available for containers.
     * Shouldn't expose any lifetime issues rvalue should be created with
     * std::move(), which can't be done accidentally.
     *
     * @param rhs Move source.
     */
    Item(Item &&rhs) = default;
    /**
     * @brief Emit destruction code in corresponding source file.
     */
    ~Item();

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
     *
     * @throws std::runtime_error On wrong key name.
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
     *
     * @throws std::runtime_error On wrong key name.
     */
    void setValue(const std::string &key, const std::string &value);
    /**
     * @brief Checks whether item has changes that require storing.
     *
     * @returns @c true if so, @c false otherwise.
     */
    bool wasChanged() const;
    /**
     * @brief Retrieves set of changes of the item.
     *
     * @returns List of item changes.
     */
    std::vector<Change> & getChanges(pk<Storage>)
    {
        return changes;
    }
    /**
     * @brief Retrieves immutable set of changes of the item.
     *
     * @returns Constant list of item changes.
     */
    const std::vector<Change> & getChanges(pk<Storage>) const
    {
        return changes;
    }
    /**
     * @brief Sets timestamp provider.
     *
     * @param getTime Function to use or empty object to reset to defaults.
     */
    static void setTimeSource(std::function<std::time_t()> getTime, pk<Tests>);

private:
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
     * @brief Whether item was changed.
     */
    bool changed;
};

#endif // SCRIBE__ITEM_HPP__
