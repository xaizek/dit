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

#ifndef SCRIBE__PROJECT_HPP__
#define SCRIBE__PROJECT_HPP__

#include <string>

#include "utils/Passkey.hpp"
#include "Config.hpp"
#include "Storage.hpp"

class Tests;

/**
 * @brief Project, parent of items.
 */
class Project
{
public:
    /**
     * @brief Initializes new project.
     *
     * @param rootDir Root directory of the project.
     *
     * @throws std::runtime_error If project directory already exists.
     */
    static void init(const std::string &rootDir);

public:
    /**
     * @brief Creates an instance of particular project.
     *
     * @param rootDir Root directory of the project.
     * @param globalConfig Global application configuration.
     */
    explicit Project(std::string rootDir, Config *globalConfig = nullptr);
    /**
     * @brief Same as normal ctor, but marks associated storage as loaded.
     *
     * @param rootDir Root directory of the project.
     * @param globalConfig Global application configuration.
     */
    explicit Project(std::string rootDir, Config *globalConfig, pk<Tests>);
    /**
     * @brief Emit destructor in the corresponding source file.
     */
    ~Project();

public:
    /**
     * @brief Checks whether project exists.
     *
     * @returns @c true if so, @c false otherwise.
     */
    bool exists() const;
    /**
     * @brief Retrieves name of the project.
     *
     * @returns The name.
     */
    std::string getName() const;
    /**
     * @brief Retrieves storage associates with this project.
     *
     * @returns The storage.
     */
    Storage & getStorage();
    /**
     * @brief Retrieves configuration of the project.
     *
     * @returns The configuration.
     */
    Config & getConfig();
    /**
     * @brief Retrieves path to directory where items are stored.
     *
     * @returns The path.
     */
    const std::string & getDataDir() const;
    /**
     * @brief Stores all project related data.
     *
     * @throws boost::filesystem::filesystem_error On issues with storage.
     * @throws std::runtime_error On data write failure.
     */
    void save();

private:
    /**
     * @brief Storage of the project.
     */
    Storage storage;
    /**
     * @brief Configuration of the project.
     */
    Config config;
    /**
     * @brief Path to root directory of the project.
     */
    const std::string rootDir;
    /**
     * @brief Path to directory containing items.
     */
    std::string dataDir;
};

#endif // SCRIBE__PROJECT_HPP__
