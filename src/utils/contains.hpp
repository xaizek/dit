// Copyright (C) 2016 xaizek <xaizek@posteo.net>
//
// This file is part of dit.
//
// dit is free software: you can redistribute it and/or modify
// it under the terms of version 3 of the GNU Affero General Public
// License as published by the Free Software Foundation.
//
// dit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dit.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DIT__UTILS__CONTAINS_HPP__
#define DIT__UTILS__CONTAINS_HPP__

#include <algorithm>
#include <string>
#include <type_traits>
#include <utility>

namespace detail {

/**
 * @brief Checks whether container contains @c find() method.
 *
 * @tparam T Type of the container.
 * @tparam A Type of the argument for the @c find().
 */
template <typename T, typename A>
struct has_find
{
    template <typename C>
    static std::true_type test(
        decltype(std::declval<C>().find(std::declval<A>())) *);
    template <typename C>
    static std::false_type test(...);

    static typename decltype(test<T>(0))::type value;
};

}

/**
 * @brief Checks whether container contains given item.
 *
 * @tparam C Type of the container.
 * @tparam T Type of the element.
 * @param c Container to examine.
 * @param val Item to look up in the container.
 *
 * @returns @c true when item is found, @c false otherwise.
 */
template <typename C, typename T>
typename std::enable_if<detail::has_find<C, T>::value, bool>::type
contains(const C &c, const T &val)
{
    return c.find(val) != c.cend();
}

/**
 * @brief Checks whether container contains given item.
 *
 * @tparam C Type of the container.
 * @tparam T Type of the element.
 * @param c Container to examine.
 * @param val Item to look up in the container.
 *
 * @returns @c true when item is found, @c false otherwise.
 */
template <typename C, typename T>
typename std::enable_if<!detail::has_find<C, T>::value, bool>::type
contains(const C &c, const T &val)
{
    return std::find(c.cbegin(), c.cend(), val) != c.cend();
}

/**
 * @brief Checks whether container contains given item.
 *
 * @param c Container to examine.
 * @param val Item to look up in the container.
 *
 * @returns @c true when item is found, @c false otherwise.
 */
template <typename T>
bool
contains(const std::string &c, const T &val)
{
    return c.find(val) != std::string::npos;
}

#endif // DIT__UTILS__CONTAINS_HPP__
