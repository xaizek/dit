// Copyright (C) 2015 xaizek <xaizek@openmailbox.org>
// Copyright (C) 2013 Eric Niebler (http://ericniebler.com/)
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

#ifndef SCRIBE__UTILS_HPP__
#define SCRIBE__UTILS_HPP__

#include <istream>
#include <iterator>
#include <stdexcept>
#include <string>

#include <boost/range.hpp>

// Heavily based on code from this post:
// http://ericniebler.com/2013/10/13/out-parameters-vs-move-semantics/

namespace detail {

class linesIterator
  : public boost::iterator_facade<
        linesIterator,
        std::string,
        std::input_iterator_tag
    >
{
public:
    linesIterator() : s(), str(), delim() {}

    linesIterator(std::istream *s, std::string *str, char delim)
        : s(s), str(str), delim(delim)
    {
        increment();
    }

private:
    friend class boost::iterator_core_access;

    void increment()
    {
        if (!std::getline(*s, *str, delim)) {
            *this = linesIterator();
        }
    }

    bool equal(const linesIterator &that) const
    {
        return str == that.str;
    }

    std::string & dereference() const
    {
        return *str;
    }

private:
    std::istream *s;
    std::string *str;
    char delim;
};

class linesRangeData
{
protected:
    std::string str;
};

using linesRangeBase = boost::iterator_range<linesIterator>;

class linesRange : private linesRangeData, public linesRangeBase
{
public:
    explicit linesRange(std::istream &s, char delim = '\n')
        : linesRangeBase(linesIterator(&s, &str, delim), linesIterator())
    {}
};

}

inline detail::linesRange
getLines(std::istream& s, char delim = '\n')
{
    return detail::linesRange(s, delim);
}

/**
 * @brief Splits string in two parts at the leftmost delimiter.
 *
 * @param s String to split.
 * @param delim Delimiter, which separates left and right parts of the string.
 *
 * @returns Pair of left and right string parts.
 *
 * @throws std::runtime_error On failure to find delimiter in the string.
 */
inline std::pair<std::string, std::string>
splitAt(const std::string &s, char delim)
{
    const std::string::size_type pos = s.find(delim);
    if (pos == std::string::npos) {
        throw std::runtime_error("Can't split " + s + " with " + delim);
    }

    return { s.substr(0, pos), s.substr(pos + 1U) };
}

namespace detail {

template <class C>
class Sorted
{
public:
    Sorted(C &&c) : c(std::move(c))
    {
        std::sort(this->c.begin(), this->c.end());
    }

    typename C::iterator begin() { return c.begin(); }
    typename C::iterator end() { return c.end(); }

private:
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
template <class C>
detail::Sorted<C> sorted(C &&c)
{
    return detail::Sorted<C>(std::move(c));
}

#endif // SCRIBE__UTILS_HPP__
