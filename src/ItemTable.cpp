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
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/range/adaptor/reversed.hpp>

#include "Item.hpp"
#include "ItemFilter.hpp"
#include "decoration.hpp"
#include "parsing.hpp"

static std::ostream & operator<<(std::ostream &os,
                              const std::vector<ColorRule::decoration> &decors);
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
    const std::string getHeading() const
    {
        return truncate(heading);
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
     * @brief Reduces width of the column by @p by positions.
     *
     * @param by Amount by which to adjust column width.
     */
    void reduceWidthBy(unsigned int by)
    {
        width -= std::min(width, by);
    }

    /**
     * @brief Retrieves printable value of the column by index.
     *
     * The value can be truncated to fit limited width, which is indicated by
     * trailing ellipsis.
     *
     * @param i Index of the value (no range checks are performed).
     *
     * @returns The value.
     */
    std::string operator[](unsigned int i) const
    {
        return truncate(values[i]);
    }

private:
    /**
     * @brief Truncates a string with ellipsis to fit into column width.
     *
     * @param s The string to truncate.
     *
     * @returns Truncated string, which is the same as @p s if it already fits.
     */
    std::string truncate(const std::string &s) const
    {
        if (s.length() <= width) {
            return s;
        }
        if (width <= 3U) {
            return std::string("...").substr(0U, width);
        }
        return s.substr(0U, width - 3U) + "...";
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

static const std::string gap = "  ";

ItemTable::ItemTable(const std::string &fmt, const std::string &colorSpec,
                     std::string sort, unsigned int maxWidth)
    : sort(std::move(sort)), maxWidth(maxWidth)
{
    for (std::string key : split(fmt, '|')) {
        std::string heading = key;
        boost::algorithm::to_upper(heading);
        boost::algorithm::trim_left_if(heading, boost::is_from_range('_', '_'));

        cols.emplace_back(std::move(key), std::move(heading));
    }

    if (!parseColorRules(colorSpec, colorRules)) {
        throw std::runtime_error("Failed to parse colorization specification.");
    }
}

ItemTable::~ItemTable()
{
}

void
ItemTable::append(Item &item)
{
    items.emplace_back(item);
}

void
ItemTable::print(std::ostream &os)
{
    sortItems();

    fillColumns();

    if (!adjustColumnsWidths()) {
        // Available width is not enough to display table.
        return;
    }

    printTableHeader(os);
    printTableRows(os);
}

void
ItemTable::sortItems()
{
    const std::vector<std::string> keys = split(sort, '|');
    for (const std::string &key : boost::adaptors::reverse(keys)) {
        std::stable_sort(items.begin(), items.end(),
                         [&key](Item &a, Item &b) {
                             return a.getValue(key) < b.getValue(key);
                         });
    }
}

void
ItemTable::fillColumns()
{
    for (Item &item : items) {
        for (Column &col : cols) {
            col.append(item.getValue(col.getKey()));
        }
    }
}

bool
ItemTable::adjustColumnsWidths()
{
    // The code below assumes that there is at least one column.
    if (cols.empty()) {
        return false;
    }

    // Calculate real width of the table.
    unsigned int realWidth = 0U;
    for (Column &col : cols) {
        realWidth += col.getWidth();
    }
    realWidth += gap.length()*(cols.size() - 1U);

    // Make ordering of columns that goes from widest to narrowest.
    std::vector<std::reference_wrapper<Column>> sorted {
        cols.begin(), cols.end()
    };
    std::sort(sorted.begin(), sorted.end(),
              [](const Column &a, const Column &b) {
                  return a.getWidth() >= b.getWidth();
              });

    // Repeatedly reduce columns until we reach target width.
    // At each iteration: reduce width of (at most all, but not necessarily) the
    // widest columns by making them at most as wide as the narrower columns
    // that directly follow them.
    while (realWidth > maxWidth) {
        unsigned int toReduce = realWidth - maxWidth;

        // Make list of the widest columns as well as figure out by which amount
        // we can adjust the width (difference between column widths).
        std::vector<std::reference_wrapper<Column>> widest;
        unsigned int maxAdj = static_cast<Column&>(sorted.front()).getWidth();
        for (Column &col : sorted) {
            const unsigned int w = col.getWidth();
            if (w != maxAdj) {
                maxAdj -= w;
                break;
            }
            widest.push_back(col);
        }

        // Reversed order of visiting to ensure that ordering invariant is
        // intact: last visited element can be reduced by smaller amount, which
        // will leave it the biggest.  Actually it doesn't matter because we
        // reach target width at the same time, still it might matter later.
        for (Column &col : boost::adaptors::reverse(widest)) {
            const unsigned int by = std::min(maxAdj, toReduce);
            col.reduceWidthBy(by);
            toReduce -= by;
        }

        // We could exhaust possibilities to reduce column width and all that's
        // left is padding between columns.
        if (maxAdj == 0) {
            break;
        }

        // Update current width of the table.
        realWidth = maxWidth + toReduce;
    }

    return realWidth <= maxWidth;
}

void
ItemTable::printTableHeader(std::ostream &os)
{
    for (Column &col : cols) {
        decorate(os, nullptr)
           << std::setw(col.getWidth()) << std::left << col.getHeading()
           << (colorRules.empty() ? decor::none : decor::def);

        if (&col != &cols.back()) {
            os << gap;
        }
    }
    os << '\n';
}

void
ItemTable::printTableRows(std::ostream &os)
{
    for (unsigned int i = 0, n = items.size(); i < n; ++i) {
        decorate(os, &items[i].get());
        for (Column &col : cols) {
            os << std::setw(col.getWidth()) << std::left << col[i];
            if (&col != &cols.back()) {
                os << gap;
            }
        }
        os << (colorRules.empty() ? decor::none : decor::def)
           << '\n';
    }
}

std::ostream &
ItemTable::decorate(std::ostream &os, Item *item)
{
    const ColorRule *match = nullptr;

    for (const ColorRule &rule : colorRules) {
        for (const Cond &cond : rule.conds) {
            if (cond.key == "!heading") {
                if (item == nullptr) {
                    match = &rule;
                    break;
                }
            } else if (item != nullptr) {
                if (ItemFilter(cond).passes(*item)) {
                    match = &rule;
                    break;
                }
            }
        }
        if (match != nullptr) {
            break;
        }
    }

    if (match != nullptr) {
        os << match->decors;
    }

    return os;
}

/**
 * @brief Applies multiple decorators to single stream.
 *
 * @param os Stream to apply decorators.
 * @param decors Decorators to process.
 *
 * @returns @p os
 */
static std::ostream &
operator<<(std::ostream &os, const std::vector<ColorRule::decoration> &decors)
{
    for (const ColorRule::decoration &decor : decors) {
        os << decor;
    }
    return os;
}

/**
 * @brief Splits string in a range-for loop friendly way.
 *
 * @param str String to split into substrings.
 * @param with Character to split at.
 *
 * @returns Array of results, empty on empty string.
 */
static std::vector<std::string>
split(const std::string &str, char with)
{
    if (str.empty()) {
        return {};
    }

    std::vector<std::string> results;
    boost::split(results, str, boost::is_from_range(with, with));
    return std::move(results);
}
