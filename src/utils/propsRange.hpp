// Copyright (C) 2015 xaizek <xaizek@posteo.net>
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

#ifndef DIT__UTILS__PROPSRANGE_HPP__
#define DIT__UTILS__PROPSRANGE_HPP__

#include <algorithm>
#include <iterator>
#include <sstream>
#include <stack>
#include <string>
#include <utility>

#include <boost/property_tree/ptree.hpp>
#include <boost/range/iterator_range.hpp>

namespace detail {

namespace pt = boost::property_tree;

/**
 * @brief Storage for range iterator data.
 */
class propsRangeData
{
public:
    /**
     * @brief Constructs data out of property tree.
     *
     * @param props Property tree to iterate over.
     */
    explicit propsRangeData(pt::ptree &props)
    {
        it.emplace(props.ordered_begin(), props.not_found());
    }

    /**
     * @brief Holds tree visit iterators as begin-end pairs.
     */
    std::stack<
        std::pair<
            pt::ptree::assoc_iterator,
            pt::ptree::assoc_iterator
        >
    > it;
    /**
     * @brief Names of all parent nodes of the current one.
     */
    std::vector<std::string> parents;
    /**
     * @brief Full name of the current node.
     */
    std::string fullName;
};


/**
 * @brief Iterator for property tree items.
 */
class propsIterator
  : public boost::iterator_facade<
        propsIterator,
        std::string,
        std::input_iterator_tag
    >
{
    friend class boost::iterator_core_access;

public:
    /**
     * @brief Constructs empty iterator ("end" iterator).
     */
    propsIterator() : rd(nullptr)
    {
    }

    /**
     * @brief Constructs non-empty iterator.
     *
     * @param rd Pointer to data storage that outlives the iterator.
     */
    propsIterator(propsRangeData *rd) : rd(rd)
    {
        // Special case of empty property tree.
        if (rd->it.top().first == rd->it.top().second) {
            rd->it.pop();
        }
        increment();
    }

private:
    /**
     * @brief Advances to the next property node.
     */
    void increment()
    {
        // Turn into "end" iterator and boil out if visit stack is empty.
        if (rd->it.empty()) {
            *this = propsIterator();
            return;
        }

        // Go down until a leaf.
        while (!rd->it.top().first->second.empty()) {
            pt::ptree::assoc_iterator &it = rd->it.top().first;

            rd->parents.emplace_back(it->first);
            rd->it.emplace(it->second.ordered_begin(), it->second.not_found());
        }

        // Format full name for the property.
        std::ostringstream oss;
        std::copy(rd->parents.cbegin(), rd->parents.cend(),
                  std::ostream_iterator<std::string>(oss, "."));
        rd->fullName = oss.str() + rd->it.top().first->first;

        // Go up until first unfinished level.
        while (!rd->it.empty() && ++rd->it.top().first == rd->it.top().second) {
            rd->it.pop();
            // Parents vector is empty for top-level elements.
            if (!rd->parents.empty()) {
                rd->parents.pop_back();
            }
        }
    }

    /**
     * @brief Checks whether two iterators are equal.
     *
     * @param that Iterator to compare against @c *this.
     *
     * @returns @c true if equal, @c false otherwise.
     */
    bool equal(const propsIterator &that) const
    {
        return rd == that.rd;
    }

    /**
     * @brief Retrieves value of a valid iterator (not "end" iterator).
     *
     * @returns The value.
     */
    std::string & dereference() const
    {
        return rd->fullName;
    }

private:
    /**
     * @brief Pointer to data storage.
     */
    propsRangeData *rd;
};

using propsRangeBase = boost::iterator_range<propsIterator>;

}

/**
 * @brief Range iterator over sorted names of property tree items.
 *
 * Each element is returned in fully qualified form: dot-separated path of
 * nodes, e.g. "root.middle.end".
 */
class propsRange : private detail::propsRangeData, public detail::propsRangeBase
{
public:
    explicit propsRange(boost::property_tree::ptree &props)
        : detail::propsRangeData(props),
          detail::propsRangeBase(detail::propsIterator(this),
                                 detail::propsIterator())
    {}
};

#endif // DIT__UTILS__PROPSRANGE_HPP__
