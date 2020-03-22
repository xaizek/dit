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

#ifndef DIT__COMPLETION_HPP__
#define DIT__COMPLETION_HPP__

#include <iosfwd>
#include <string>
#include <vector>

class Item;
class Storage;

/**
 * @brief Lists all projects.
 *
 * @param projectsDir Base directory for projects.
 *
 * @returns Names of discovered projects with prepended dot (.).
 */
std::vector<std::string> listProjects(const std::string &projectsDir);

/**
 * @brief Completes IDs from the storage.
 *
 * @param storage Source of IDs.
 * @param os Stream to output completion.
 *
 * @returns Application exit code.
 */
int completeIds(Storage &storage, std::ostream &os);

/**
 * @brief Completes keys from single element.
 *
 * @param item Source of key names.
 * @param os Stream to output completion.
 * @param args Existing arguments of the command (used to remove duplicates).
 *
 * @returns Application exit code.
 */
int completeKeys(Item &item, std::ostream &os,
                 const std::vector<std::string> &args);

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
