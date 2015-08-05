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

#include <memory>
#include <utility>

#include "Item.hpp"

Item &
Storage::create()
{
    decltype(items)::iterator it;
    bool inserted;
    std::tie(it, inserted) = items.emplace("fid", Item("fid"));
    assert(inserted && "Duplicated item id");

    return it->second;
}

std::vector<std::reference_wrapper<Item>>
Storage::list()
{
    // TODO: check if list wasn't loaded from physical storage and if so load it
    //       now.

    std::vector<std::reference_wrapper<Item>> list;
    list.reserve(items.size());
    for (auto &e : items) {
        list.emplace_back(e.second);
    }
    return list;
}
