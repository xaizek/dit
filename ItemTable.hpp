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

#ifndef SCRIBE__ITEMTABLE_HPP__
#define SCRIBE__ITEMTABLE_HPP__

#include <functional>
#include <string>
#include <vector>

class Item;

/**
 * @brief Item table formatter and printer.
 *
 * Format and sorting are configurable via constructors parameters.
 */
class ItemTable
{
    class Column;

public:
    /**
     * @brief Constructs the table formatter.
     *
     * @param fmt Format specification: <field>|<field>...
     * @param sort Multi-key sorting specification: <field>|<field>...
     */
    ItemTable(const std::string &fmt, std::string sort);
    /**
     * @brief To emit destructing code in corresponding source file.
     */
    ~ItemTable();

public:
    /**
     * @brief Adds item to the table.
     *
     * @param item Item to add.
     */
    void append(Item &item);
    /**
     * @brief Prints table on standard output.
     */
    void print();

private:
    /**
     * @brief Sorting specification.
     */
    const std::string sort;
    /**
     * @brief List of columns of the table (built from the format).
     */
    std::vector<Column> cols;
    /**
     * @brief List of items to sort.
     */
    std::vector<std::reference_wrapper<Item>> items;
};

#endif // SCRIBE__ITEMTABLE_HPP__
