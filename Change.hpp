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

#ifndef SCRIBE__CHANGE_HPP__
#define SCRIBE__CHANGE_HPP__

#include <ctime>

#include <string>
#include <utility>

/**
 * @brief Represents single change of Item's description.
 */
class Change
{
public:
    /**
     * @brief Constructs a change.
     *
     * @param timestamp When the change took place.
     * @param key What was changed.
     * @param value What's new value.
     */
    Change(std::time_t timestamp, std::string key, std::string value)
        : timestamp(timestamp), key(std::move(key)), value(std::move(value))
    {
    }

public:
    /**
     * @brief Retrieves time at which the change was made.
     *
     * @returns The time.
     */
    std::time_t getTimestamp() const { return timestamp; }
    /**
     * @brief Retrieves name of the field described by this change.
     *
     * @returns The name.
     */
    const std::string & getKey() const { return key; }
    /**
     * @brief Retrieves value of the field described by this change.
     *
     * @returns The value.
     */
    const std::string & getValue() const { return value; }

private:
    /**
     * @brief When the change was made.
     */
    std::time_t timestamp;
    /**
     * @brief Name of the field.
     */
    const std::string key;
    /**
     * @brief Value of the field.
     */
    const std::string value;
};

#endif // SCRIBE__CHANGE_HPP__
