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

#ifndef SCRIBE__COMPLETION_HPP__
#define SCRIBE__COMPLETION_HPP__

#include <iosfwd>
#include <string>
#include <vector>

class Storage;

/**
 * @brief Implementation of completion of item keys in the storage.
 *
 * @param storage Source of the keys (taken from all the elements).
 * @param os Stream to output completion.
 * @param args Existing arguments of the command (used to remove duplicates).
 *
 * @returns Application exit code.
 */
int completeKeys(Storage &storage, std::ostream &os,
                 const std::vector<std::string> &args);

#endif // SCRIBE__COMPLETION_HPP__
