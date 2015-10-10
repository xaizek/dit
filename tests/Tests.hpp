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

#ifndef SCRIBE__TESTS_HPP__
#define SCRIBE__TESTS_HPP__

#include <string>

class Item;

/**
 * @brief Attorney for accessing testing interface of the application classes.
 *
 * Attorney is from Attorney-Client idiom.
 */
class Tests
{
public:
    /**
     * @brief Makes an instance of @c Item, which is not creatable otherwise.
     *
     * @param id Id for the new item.
     *
     * @returns The item.
     */
    static Item makeItem(std::string id);
};

#endif // SCRIBE__TESTS_HPP__
