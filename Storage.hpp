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

class Item;

/**
 * @brief Storage for a set of items.
 */
class Storage
{
public:
    /**
     * @brief Creates new item.
     *
     * @returns Reference to newly created item.
     */
    Item & create();
    /**
     * @brief Lists all available items.
     *
     * @returns Snapshot of current list of items.
     */
    std::vector<std::reference_wrapper<Item>> list();

private:
    /**
     * @brief Items known to the storage.
     */
    std::map<std::string, Item> items;
};

#endif // SCRIBE__STORAGE_HPP__
