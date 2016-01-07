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

#ifndef DIT__TESTS_HPP__
#define DIT__TESTS_HPP__

#include <ctime>

#include <functional>
#include <iosfwd>
#include <memory>
#include <string>

class Config;
class Item;
class Project;
class Storage;

/**
 * @brief Attorney for accessing testing interface of the application classes.
 *
 * Attorney is from Attorney-Client idiom.
 */
class Tests
{
public:
    /**
     * @brief Disables adding escape sequences to output.
     */
    static void disableDecorations();

    /**
     * @brief Set custom streams for command output.
     *
     * @param out Output stream.
     * @param err Error stream.
     */
    static void setStreams(std::ostream &out, std::ostream &err);

    /**
     * @brief Sets timestamp provider for Item and Change objects.
     *
     * @param getTime New provider.
     */
    static void setTimeSource(std::function<std::time_t()> getTime);
    /**
     * @brief Resets timestamp provider to its default value.
     */
    static void resetTimeSource();

    /**
     * @brief Makes an instance of @c Project.
     *
     * @returns The instance.
     */
    static std::unique_ptr<Project> makeProject();

    /**
     * @brief Makes an instance of @c Item, which is not creatable otherwise.
     *
     * @param id Id for the new item.
     *
     * @returns The item.
     */
    static Item makeItem(std::string id);
    /**
     * @brief Inserts the item into the storage.
     *
     * @param storage Storage to place @p item into.
     * @param item Item to put into the storage.
     */
    static void storeItem(Storage &storage, Item item);

    /**
     * @brief Retrieves whether configuration has any unsaved changes.
     *
     * @returns @c true if so, otherwise @c false is returned.
     */
    static bool configIsModified(Config &config);
};

#endif // DIT__TESTS_HPP__
