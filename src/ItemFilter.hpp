// Copyright (C) 2015 xaizek <xaizek@posteo.net>
//
// This file is part of dit.
//
// dit is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dit.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DIT__ITEMFILTER_HPP__
#define DIT__ITEMFILTER_HPP__

#include <functional>
#include <string>
#include <vector>

struct Cond;

class Item;

/**
 * @brief Checks items for satisfying set of constraints.
 *
 * Supported operations include: ==, !=.
 * Form of each expression: <field> <op> <value>.
 */
class ItemFilter
{
    /**
     * @brief Type of function that is used to query field value.
     *
     * A field can be expanded to zero, one or multiple values and each value
     * will be matched individually.
     */
    using accessor_f = std::vector<std::string>(const std::string &key);

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

    /**
     * @brief Checks whether item represented by its fields passes the filter.
     *
     * This is a generalized version, which allows for different representations
     * of items.
     *
     * @param accessor Accessor of item data (by field name).
     *
     * @returns @c true if it passes, and @c false otherwise.
     */
    bool passes(const std::function<accessor_f> &accessor) const;

    /**
     * @brief Checks whether item represented by its fields passes the filter.
     *
     * @param accessor Accessor of item data (by field name).
     * @param error[out] Storage for error message.
     *
     * @returns @c true if it passes, and @c false otherwise.
     */
    bool passes(const std::function<accessor_f> &accessor,
                std::string &error) const;

private:
    /**
     * @brief Set of constraints.
     */
    std::vector<Cond> conds;
};

#endif // DIT__ITEMFILTER_HPP__
