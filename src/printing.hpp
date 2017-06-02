// Copyright (C) 2016 xaizek <xaizek@openmailbox.org>
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

#ifndef DIT__PRINTING_HPP__
#define DIT__PRINTING_HPP__

#include <ostream>
#include <string>

#include "utils/contains.hpp"
#include "decoration.hpp"

/**
 * @brief Strong typing of string with command name for output stream overload.
 */
class Cmd
{
public:
    const std::string &data;
};

/**
 * @brief Strong typing of string with key name for output stream overload.
 */
class Key
{
public:
    const std::string &data;
};

/**
 * @brief Strong typing of string with value for output stream overload.
 */
class Value
{
public:
    const std::string &data;
};

/**
 * @brief Formatted command name printer.
 *
 * @param os Stream to output formatted data to.
 * @param cmd Command name.
 *
 * @returns @p os.
 */
inline std::ostream &
operator<<(std::ostream &os, const Cmd &cmd)
{
    return os << (decor::bold << cmd.data);
}

/**
 * @brief Formatted key name printer.
 *
 * @param os Stream to output formatted data to.
 * @param key Key name.
 *
 * @returns @p os.
 */
inline std::ostream &
operator<<(std::ostream &os, const Key &key)
{
    return os << (decor::bold << key.data);
}

/**
 * @brief Formatted value printer.
 *
 * @param os Stream to output formatted data to.
 * @param val Value.
 *
 * @returns @p os.
 */
inline std::ostream &
operator<<(std::ostream &os, const Value &val)
{
    return os << (contains(val.data, '\n') ? ":\n" : ": ")
              << val.data;
}

#endif // DIT__PRINTING_HPP__
