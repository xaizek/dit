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

#include <string>

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
     * @param id Id of the item.
     */
    Item(std::string id);

public:
    /**
     * @brief Retrieves item id.
     *
     * @returns The id.
     */
    std::string getId() const;

private:
    /**
     * @brief Id of this item.
     */
    const std::string id;
};

#endif // SCRIBE__ITEM_HPP__
