// Copyright (C) 2015 xaizek <xaizek@posteo.net>
//
// This file is part of dit.
//
// dit is free software: you can redistribute it and/or modify
// it under the terms of version 3 of the GNU Affero General Public
// License as published by the Free Software Foundation.
//
// dit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dit.  If not, see <http://www.gnu.org/licenses/>.

#include "Item.hpp"

#include <functional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/range/adaptor/reversed.hpp>

#include "utils/Passkey.hpp"
#include "utils/time.hpp"
#include "Change.hpp"
#include "Storage.hpp"
#include "parsing.hpp"

/**
 * @brief Function used to obtain current timestamp.
 */
static std::function<std::time_t()> getTime =
    [](){
        return std::time(nullptr);
    };

bool
Item::isValidKeyName(const std::string &name, bool forWrite, std::string &error)
{
    auto iter = name.cbegin();
    auto end = name.cend();
    if (!parseKeyName(iter, end)) {
        error = "Invalid key name at " + std::string(&*iter);
        return false;
    }

    if (forWrite && name[0] == '_') {
        error = "The key is read-only.";
        return false;
    }

    return true;
}

Item::Item(Storage &storage, std::string id, bool exists, pk<Storage>)
    : StorageBacked<Item>(!exists), storage(storage), id(std::move(id))
{
    // Count item creation as a modification.
    if (!exists) {
        markModified();
    }
}

Item::Item(Storage &storage, std::string id, pk<Tests>)
    : StorageBacked<Item>(true), storage(storage), id(std::move(id))
{
}

Item::~Item()
{
}

const std::string &
Item::getId() const
{
    return id;
}

std::string
Item::getValue(const std::string &key)
{
    std::string error;
    if (!isValidKeyName(key, false, error)) {
        throw std::runtime_error(error);
    }

    // Handle pseudo fields.
    if (key == "_id") {
        return getId();
    } else if(key == "_created") {
        if (changes.empty()) {
            return std::string();
        }

        return timeToString(changes.front().getTimestamp());
    } else if(key == "_changed") {
        if (changes.empty()) {
            return std::string();
        }

        return timeToString(changes.back().getTimestamp());
    }

    const Change *const change = getLatestChange(key);
    return (change != nullptr) ? change->getValue() : std::string();
}

std::set<std::string>
Item::listRecordNames()
{
    ensureLoaded();

    std::set<std::string> names;
    for (const Change &c : changes) {
        if (!c.getValue().empty()) {
            names.insert(c.getKey());
        } else {
            names.erase(c.getKey());
        }
    }
    return names;
}

void
Item::load()
{
    storage.fill(*this, {});

    for (int i = 0, c = changes.size(); i < c - 1; ++i) {
        if (changes[i].getTimestamp() > changes[i + 1].getTimestamp()) {
            throw std::logic_error("Change set for " + id + " is not sorted.");
        }
    }
}

void
Item::setValue(const std::string &key, const std::string &value)
{
    std::string error;
    if (!isValidKeyName(key, true, error)) {
        throw std::runtime_error(error);
    }

    const std::time_t timestamp = getTime();

    if (Change *const change = getLatestChange(key)) {
        if (change->getValue() == value) {
            return;
        }

        if (change->getTimestamp() == timestamp) {
            // Update previous change with new value.
            *change = Change(timestamp, key, value);

            if (Change *const prev = getLatestChange(key, change)) {
                if (prev->getValue() == value) {
                    // Remove the change that matches old value.
                    changes.erase(changes.begin() + (change - &changes[0]));
                }
            } else if (value.empty()) {
                // Remove the change that matches old value.
                changes.erase(changes.begin() + (change - &changes[0]));
            }

            markModified();
            return;
        }
    } else if (value.empty()) {
        return;
    }

    changes.emplace_back(timestamp, key, value);
    markModified();
}

Change *
Item::getLatestChange(const std::string &key)
{
    ensureLoaded();

    for (Change &c : boost::adaptors::reverse(changes)) {
        if (c.getKey() == key) {
            return &c;
        }
    }
    return nullptr;
}

Change *
Item::getLatestChange(const std::string &key, Change *before)
{
    for (Change *c = before - 1; c >= &changes[0]; --c) {
        if (c->getKey() == key) {
            return c;
        }
    }
    return nullptr;
}

bool
Item::wasChanged() const
{
    return isModified();
}

const std::vector<Change> &
Item::getChanges()
{
    ensureLoaded();
    return changes;
}

std::vector<Change> &
Item::getChanges(pk<Storage>)
{
    return changes;
}

const std::vector<Change> &
Item::getChanges(pk<Storage>) const
{
    return changes;
}

void
Item::setTimeSource(std::function<std::time_t()> getTime, pk<Tests>)
{
    if (!getTime) {
        ::getTime = [](){ return std::time(nullptr); };
    } else {
        ::getTime = getTime;
    }
}
