// Copyright (C) 2015 xaizek <xaizek@posteo.net>
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

#ifndef DIT__FILE_FORMAT_HPP__
#define DIT__FILE_FORMAT_HPP__

#include <iosfwd>
#include <vector>

class Change;

/**
 * @brief Converts text data read from @p s into set of changes.
 *
 * @param s Stream to read data from.
 * @param changes Storage for read data.
 *
 * @returns @p s.
 *
 * @throws std::runtime_error On broken textual representation.
 */
std::istream & operator>>(std::istream &s, std::vector<Change> &changes);

/**
 * @brief Writes @p changes in the stream @p s in text form.
 *
 * @param s Output stream for the data.
 * @param changes Changes to write.
 *
 * @returns @p s.
 */
std::ostream & operator<<(std::ostream &s, const std::vector<Change> &changes);

#endif // DIT__FILE_FORMAT_HPP__
