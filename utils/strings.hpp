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

#ifndef SCRIBE__UTILS__STRINGS_HPP__
#define SCRIBE__UTILS__STRINGS_HPP__

#include <stdexcept>
#include <string>
#include <utility>

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

#endif // SCRIBE__UTILS__STRINGS_HPP__
