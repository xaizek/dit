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

#ifndef SCRIBE__PARSING_HPP__
#define SCRIBE__PARSING_HPP__

#include <string>

/**
 * @brief Enumerations of supported operations.
 */
enum class Op
{
    eq,           /**< @brief Check for equality. */
    ne,           /**< @brief Check for inequality. */
    iccontains,   /**< @brief Look up substring ignoring case. */
    icnotcontain, /**< @brief Look up substring not ignoring case. */
};

/**
 * @brief Single condition expression.
 */
struct Cond
{
    std::string key;   /**< @brief Name of the key. */
    Op op;             /**< @brief Operation to use for comparison. */
    std::string value; /**< @brief Value to match against. */
};

/**
 * @brief Parses key name to validate it.
 *
 * @param[in,out] iter Parsing position (updated on failure as well).
 * @param end Final parsing position.
 *
 * @returns @c true on successful parsing and @c false otherwise.
 */
bool parseKeyName(std::string::const_iterator &iter,
                  std::string::const_iterator end);

/**
 * @brief Parses line into @c Cond instance.
 *
 * @param[in,out] iter Parsing position (updated on failure as well).
 * @param end Final parsing position.
 * @param[out] cond Buffer for the result.
 *
 * @returns @c true on successful parsing and @c false otherwise.
 */
bool parseCond(std::string::const_iterator &iter,
               std::string::const_iterator end, Cond &cond);

#endif // SCRIBE__PARSING_HPP__