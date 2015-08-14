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

#include <cstddef>
#include <ctime>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/range/adaptor/reversed.hpp>

#include "Storage.hpp"

Item::Item(Storage &storage, std::string id, bool exists)
    : storage(storage), id(std::move(id)), loaded(!exists), changed(false)
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
    const Change *const change = getLatestChange(key);
    return (change != nullptr) ? change->getValue() : std::string();
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
    storage.fill(*this);

    for (int i = 0, c = changes.size(); i < c - 1; ++i) {
        if (changes[i].getTimestamp() > changes[i + 1].getTimestamp()) {
            throw std::logic_error("Change set for " + id + " is not sorted.");
        }
    }
}

void
Item::setValue(const std::string &key, const std::string &value)
{
    const std::time_t timestamp = std::time(NULL);

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
