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

#ifndef DIT__CONFIG_HPP__
#define DIT__CONFIG_HPP__

#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include "utils/Passkey.hpp"
#include "StorageBacked.hpp"

class Tests;

/**
 * @brief Abstraction over configuration storage.
 *
 * Keys that start with exclamation mark ("!") are builtin ones and shouldn't
 * normally be touched from the outside.
 */
class Config : private StorageBacked<Config>
{
    friend class StorageBacked<Config>;

    using StorageBacked<Config>::isModified;

public:
    /**
     * @brief Constructs configuration for file specified by @p path.
     *
     * @param path Associated project.
     * @param parent Parent configuration used as a fallback for unknown values.
     */
    explicit Config(std::string path, Config *parent = nullptr);
    /**
     * @brief Copying is forbidden.
     *
     * @param rhs Unused source object.
     */
    Config(const Config &rhs) = delete;
    /**
     * @brief Moving is allowed.
     *
     * @param rhs Source object for moving.
     */
    Config(Config &&rhs) = default;
    /**
     * @brief Assigning is forbidden.
     *
     * @param rhs Unused source object.
     *
     * @returns @c *this.
     */
    Config & operator=(const Config &rhs) = delete;

public:
    /**
     * @brief Retrieves value of existing @p key.
     *
     * @param key Name of the key.
     *
     * @returns The value.
     *
     * @throws boost::property_tree::ptree_bad_path On inexistent key.
     */
    std::string get(const std::string &key);
    /**
     * @brief Retrieves value of existing @p key or default value.
     *
     * @param key Key to retrieve.
     * @param def Value to be used for inexistent @p key.
     *
     * @returns The value or @p def.
     */
    std::string get(const std::string &key, const std::string &def);
    /**
     * @brief Lists all existing keys in alphabetical order.
     *
     * @param path Starting path in the configuration.
     *
     * @returns The list (items are relative to path).
     */
    std::vector<std::string> list(const std::string &path = {});
    /**
     * @brief Assigns value to the @p key.
     *
     * @param key Key to change.
     * @param val New value.
     */
    void set(const std::string &key, const std::string &val);
    /**
     * @brief Stores in-memory configuration to permanent storage.
     */
    virtual void save() override;

    /**
     * @brief Retrieves whether configuration has any unsaved changes.
     *
     * @returns @c true if so, otherwise @c false is returned.
     */
    bool isModified(pk<Tests>) const;

private:
    /**
     * @brief Fills in in-memory representation.
     */
    void load();

private:
    /**
     * @brief Path to configuration file.
     */
    const std::string path;
    /**
     * @brief Parent configuration used as a fallback for unknown values.
     */
    Config *const parent;
    /**
     * @brief In-memory storage of the configuration.
     */
    boost::property_tree::ptree props;
};

#endif // DIT__CONFIG_HPP__
