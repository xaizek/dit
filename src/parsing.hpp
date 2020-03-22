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

#ifndef DIT__PARSING_HPP__
#define DIT__PARSING_HPP__

#include <functional>
#include <iosfwd>
#include <string>
#include <vector>

namespace decor {
    class Decoration;
}

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
    std::string str;   /**< @brief Original string representation. */
    std::string key;   /**< @brief Name of the key. */
    Op op;             /**< @brief Operation to use for comparison. */
    std::string value; /**< @brief Value to match against. */
};

/**
 * @brief Describes table row decoration rule.
 */
struct ColorRule
{
    /**
     * @brief Conditions by which rule is chosen.
     *
     * Rule is matched if any of these match.
     */
    std::vector<Cond> conds;
    /**
     * @brief Decorations to apply according to this rule.
     */
    std::vector<const decor::Decoration *> decors;
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

/**
 * @brief Parses paired arguments turning "field: a val" into "field='a val'".
 *
 * @param args Original arguments.
 *
 * @returns Processed arguments with concatenated items.
 */
std::vector<std::string> parsePairedArgs(const std::vector<std::string> &args);

/**
 * @brief Parses string into set of color rules.
 *
 * @param spec Rules specification.
 * @param[out] colorRules Buffer for parsing results.
 *
 * @returns @c true on successful parsing and @c false otherwise.
 */
bool parseColorRules(const std::string &spec,
                     std::vector<ColorRule> &colorRules);

#endif // DIT__PARSING_HPP__
