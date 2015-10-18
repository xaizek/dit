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

#include "Item.hpp"

#include <ctime>

#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <boost/range/adaptor/reversed.hpp>

#include "utils/Passkey.hpp"
#include "Change.hpp"
#include "Storage.hpp"
#include "parsing.hpp"

/**
 * @brief std::put_time emulation with a tuple (available since GCC 5.0).
 */
using put_time = std::tuple<const std::tm *, const char *>;

static std::string timeToString(std::time_t t);
static std::ostream & operator<<(std::ostream &os, const put_time &pt);

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
    : LazyLoadable<Item>(!exists), storage(storage), id(std::move(id)),
      changed(false)
{
}

Item::Item(std::string id, pk<Tests>)
    : LazyLoadable<Item>(true), storage(storage), id(std::move(id)),
      changed(false)
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

/**
 * @brief Converts time since epoch into local time in ISO 8601 format.
 *
 * @param t Time to convert into string.
 *
 * @returns String representation of @p t.
 */
static std::string
timeToString(std::time_t t)
{
    std::ostringstream oss;
    oss << put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

/**
 * @brief Expands put_time data into a string and prints it out.
 *
 * @param os Stream to print formated time onto.
 * @param pt put_time manipulator emulation.
 *
 * @returns @p os.
 */
static std::ostream &
operator<<(std::ostream &os, const put_time &pt)
{
    char buf[128];
    std::strftime(buf, sizeof(buf), std::get<1>(pt), std::get<0>(pt));
    return os << buf;
}

std::set<std::string>
Item::listRecordNames()
{
    ensureLoaded();

    std::set<std::string> names;
    for (const Change &c : changes) {
        names.insert(c.getKey());
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
            }

            changed = true;
            return;
        }
    }

    changes.emplace_back(timestamp, key, value);
    changed = true;
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
    return changed;
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
