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

#ifndef SCRIBE__UTILS__CONTAINERS_HPP__
#define SCRIBE__UTILS__CONTAINERS_HPP__

#include <algorithm>
#include <utility>

namespace detail {

/**
 * @brief Wrapper and storage for sorted container.
 *
 * @tparam C Type of container.
 */
template <class C>
class Sorted
{
public:
    /**
     * @brief Constructs the instance by sorting the container.
     *
     * @param c Container to sort.
     */
    Sorted(C &&c) : c(std::move(c))
    {
        std::sort(this->c.begin(), this->c.end());
    }

public:
    /**
     * @brief Retrieves begin iterator of sorted container.
     *
     * @returns The iterator.
     */
    typename C::iterator begin() { return c.begin(); }

    /**
     * @brief Retrieves end iterator of sorted container.
     *
     * @returns The iterator.
     */
    typename C::iterator end() { return c.end(); }

private:
    /**
     * @brief Sorted container.
     */
    C c;
};

}

/**
 * @brief Adapter for iterating container elements in sorted order.
 *
 * @tparam C Type of container.
 * @param c Container itself.
 *
 * @returns Object suitable to be used in range for loop.
 */
template <class C> inline detail::Sorted<C>
sorted(C &&c)
{
    return detail::Sorted<C>(std::move(c));
}

#endif // SCRIBE__UTILS__CONTAINERS_HPP__
