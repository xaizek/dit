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

#ifndef DIT__UTILS__ARGS_HPP__
#define DIT__UTILS__ARGS_HPP__

#include <string>
#include <vector>

#include <boost/tokenizer.hpp>

/**
 * @brief Tokenize the command line, respecting escapes and quotes.
 *
 * @param line Line to parse.
 *
 * @returns Array of arguments.
 */
inline std::vector<std::string>
breakIntoArgs(const std::string &line)
{
    boost::escaped_list_separator<char> sep("\\", " ", "\"'");
    boost::tokenizer<boost::escaped_list_separator<char>> tok(line, sep);
    return { tok.begin(), tok.end() };
}

#endif // DIT__UTILS__ARGS_HPP__
