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
#include <string>

#include <boost/range.hpp>

// TODO: add missing comments here

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

#endif // SCRIBE__UTILS_HPP__
