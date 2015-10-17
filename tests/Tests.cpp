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

#include "Tests.hpp"

#include <string>
#include <utility>

#include "Change.hpp"
#include "Item.hpp"
#include "Storage.hpp"

Item
Tests::makeItem(std::string id)
{
    return Item(std::move(id), {});
}

void
Tests::storeItem(Storage &storage, Item item)
{
    storage.put(std::move(item), {});
}