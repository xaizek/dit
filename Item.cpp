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

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/range/adaptor/reversed.hpp>

#include "Storage.hpp"

Item::Item(Storage &storage, std::string id)
    : storage(storage), id(std::move(id)), loaded(false)
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
    ensureLoaded();

    for (const Change &c : boost::adaptors::reverse(changes)) {
        if (c.getKey() == key) {
            return c.getValue();
        }
    }

    return {};
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
Item::ensureLoaded()
{
    if (!loaded) {
        load();
        loaded = true;
    }
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
