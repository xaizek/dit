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

#ifndef SCRIBE__UTILS__GETLINES_HPP__
#define SCRIBE__UTILS__GETLINES_HPP__

#include <istream>
#include <iterator>
#include <string>

#include <boost/range.hpp>

// See: http://ericniebler.com/2013/10/13/out-parameters-vs-move-semantics/

namespace detail {

/**
 * @brief Iterator over lines in a stream.
 */
class linesIterator
  : public boost::iterator_facade<
        linesIterator,
        std::string,
        std::input_iterator_tag
    >
{
    friend class boost::iterator_core_access;

public:
    /**
     * @brief Constructs empty ("end") iterator.
     */
    linesIterator() : s(nullptr), str(nullptr), delim('\0') {}

    /**
     * @brief Constructs non-empty iterator.
     *
     * @param s Stream to read from.
     * @param str Storage for current line.
     * @param delim Line delimiter.
     */
    linesIterator(std::istream *s, std::string *str, char delim)
        : s(s), str(str), delim(delim)
    {
        increment();
    }

private:
    /**
     * @brief Advances to the next line in the stream.
     */
    void increment()
    {
        if (!std::getline(*s, *str, delim)) {
            *this = linesIterator();
        }
    }

    /**
     * @brief Checks whether two iterators are equal.
     *
     * @param that Iterator to compare @c *this against.
     *
     * @returns @c true if they are equal, @c false otherwise.
     */
    bool equal(const linesIterator &that) const
    {
        return str == that.str;
    }

    /**
     * @brief Retrieves reference to current line.
     *
     * @returns The line.
     */
    std::string & dereference() const
    {
        return *str;
    }

private:
    /**
     * @brief Source stream.
     */
    std::istream *s;
    /**
     * @brief Line storage.
     */
    std::string *str;
    /**
     * @brief String delimiter.
     */
    char delim;
};

/**
 * @brief Storage for iterator data.
 */
class linesRangeData
{
protected:
    /**
     * @brief Current line.
     */
    std::string str;
};

using linesRangeBase = boost::iterator_range<linesIterator>;

/**
 * @brief Range iterator over stream lines.
 */
class linesRange : private linesRangeData, public linesRangeBase
{
public:
    /**
     * @brief Constructs range iterator.
     *
     * @param s Stream to read lines from.
     * @param delim Lines separator.
     */
    explicit linesRange(std::istream &s, char delim = '\n')
        : linesRangeBase(linesIterator(&s, &str, delim), linesIterator())
    {
    }
};

}

/**
 * @brief Makes range iterator for iterating over lines read from the stream.
 *
 * @param s Stream to break into lines.
 * @param delim Lines separator.
 *
 * @returns Range iterator for lines of the stream.
 */
inline detail::linesRange
getLines(std::istream& s, char delim = '\n')
{
    return detail::linesRange(s, delim);
}

#endif // SCRIBE__UTILS__GETLINES_HPP__
