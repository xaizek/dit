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

#include "ItemTable.hpp"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iterator>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/range/adaptor/reversed.hpp>

#include "Item.hpp"
#include "decoration.hpp"

static std::vector<std::string> split(const std::string &str, char with);

/**
 * @brief Helper class that represents single column of a table.
 */
class ItemTable::Column
{
public:
    /**
     * @brief Constructs empty column.
     *
     * @param key Key of the column.
     * @param heading Key of the column.
     */
    explicit Column(std::string key, std::string heading)
        : key(std::move(key)), heading(std::move(heading)),
          width(this->heading.size())
    {
    }

public:
    /**
     * @brief Retrieves key of the column.
     *
     * @returns The key.
     */
    const std::string & getKey() const
    {
        return key;
    }

    /**
     * @brief Retrieves heading of the column.
     *
     * @returns The heading.
     */
    const std::string & getHeading() const
    {
        return heading;
    }

    /**
     * @brief Adds the value to the column.
     *
     * @param val Value to add.
     */
    void append(std::string val)
    {
        width = std::max(width, static_cast<unsigned int>(val.size()));
        values.emplace_back(std::move(val));
    }

    /**
     * @brief Retrieves widths of the column.
     *
     * @returns The width.
     */
    unsigned int getWidth() const
    {
        return width;
    }

    /**
     * @brief Retrieves value of the column by index.
     *
     * @param i Index of the value.
     *
     * @returns The value.
     */
    const std::string & operator[](unsigned int i) const
    {
        return values[i];
    }

private:
    /**
     * @brief Key of the column.
     */
    const std::string key;
    /**
     * @brief Title of the column for printing.
     */
    const std::string heading;
    /**
     * @brief Width of the column.
     */
    unsigned int width;
    /**
     * @brief Contents of the column.
     */
    std::vector<std::string> values;
};

ItemTable::ItemTable(const std::string &fmt, std::string sort)
    : sort(std::move(sort))
{
    for (std::string key : split(fmt, '|')) {
        std::string heading = key;
        boost::algorithm::to_upper(heading);
        boost::algorithm::trim_left_if(heading, boost::is_from_range('_', '_'));

        cols.emplace_back(std::move(key), std::move(heading));
    }
}

ItemTable::~ItemTable()
{
}

void ItemTable::append(Item &item)
{
    items.emplace_back(item);
}

void ItemTable::print(std::ostream &os)
{
    // Ensure items are in correct order.
    const std::vector<std::string> keys = split(sort, '|');
    for (const std::string &key : boost::adaptors::reverse(keys)) {
        std::stable_sort(items.begin(), items.end(),
                         [&key](Item &a, Item &b)
                         {
                             return a.getValue(key) < b.getValue(key);
                         });
    }

    // Populate columns with items data.
    for (Item &item : items) {
        for (Column &col : cols) {
            col.append(item.getValue(col.getKey()));
        }
    }

    const std::string gap = "  ";

    // Print table heading.
    for (Column &col : cols) {
        os << decor::cyan_fg << decor::inv << decor::bold
           << std::setw(col.getWidth()) << std::left << col.getHeading()
           << decor::def
           << gap;
    }
    os << '\n';

    // Print table lines.
    for (unsigned int i = 0, n = items.size(); i < n; ++i) {
        for (Column &col : cols) {
            os << std::setw(col.getWidth()) << std::left << col[i] << gap;
        }
        os << '\n';
    }
}

/**
 * @brief Splits string in a range-for loop friendly way.
 *
 * @param str String to split into substrings.
 * @param with Character to split at.
 *
 * @returns Array of results.
 */
static std::vector<std::string>
split(const std::string &str, char with)
{
    std::vector<std::string> results;
    boost::split(results, str, boost::is_from_range(with, with));
    return std::move(results);
}
