// Copyright (C) 2015 xaizek <xaizek@openmailbox.org>
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

#ifndef DIT__COMPLETION_HPP__
#define DIT__COMPLETION_HPP__

#include <iosfwd>
#include <string>
#include <vector>

class Storage;

/**
 * @brief Completes item keys in the storage.
 *
 * @param storage Source of the keys (taken from all the elements).
 * @param os Stream to output completion.
 *
 * @returns Application exit code.
 */
int completeKeys(Storage &storage, std::ostream &os);

/**
 * @brief Completes item keys in the storage.
 *
 * Doesn't complete keys that are found among @p args.
 *
 * @param storage Source of the keys (taken from all the elements).
 * @param os Stream to output completion.
 * @param args Existing arguments of the command (used to remove duplicates).
 *
 * @returns Application exit code.
 */
int completeKeys(Storage &storage, std::ostream &os,
                 const std::vector<std::string> &args);

/**
 * @brief Completes key values.
 *
 * @param storage Source of the keys (taken from all the elements).
 * @param os Stream to output completion.
 * @param key Key, whose values to be completed.
 *
 * @returns Application exit code.
 */
int completeValues(Storage &storage, std::ostream &os, const std::string &key);

#endif // DIT__COMPLETION_HPP__
