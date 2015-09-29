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

#ifndef SCRIBE__ITEMFILTER_HPP__
#define SCRIBE__ITEMFILTER_HPP__

#include <string>
#include <vector>

class Cond;
class Item;

/**
 * @brief Checks items for satisfying set of constraints.
 *
 * Supported operations include: ==, !=.
 * Form of each expression: <field> <op> <value>.
 */
class ItemFilter
{
public:
    /**
     * @brief Constructs the filter out of conditions in textual form.
     *
     * @param exprs Set of expressions to check against.
     */
    explicit ItemFilter(const std::vector<std::string> &exprs);
    /**
     * @brief Constructs the filter out of single condition.
     *
     * @param cond
     */
    explicit ItemFilter(Cond cond);
    /**
     * @brief Emit destructor code in corresponding source file.
     */
    ~ItemFilter();

public:
    /**
     * @brief Checks whether given item passes the filter.
     *
     * @param item Item to check for compliance.
     *
     * @returns @c true if it passes, and @c false otherwise.
     */
    bool passes(Item &item) const;

private:
    /**
     * @brief Set of constraints.
     */
    std::vector<Cond> conds;
};

#endif // SCRIBE__ITEMFILTER_HPP__
