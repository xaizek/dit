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

#ifndef SCRIBE__SCRIBE_HPP__
#define SCRIBE__SCRIBE_HPP__

#include <memory>
#include <string>
#include <vector>

class Config;

/**
 * @brief Application implementation.
 */
class Scribe
{
public:
    /**
     * @brief Constructs main application class.
     *
     * @param argc Number of application arguments.
     * @param argv Application arguments.
     *
     * @throws std::runtime_error on broken argument list.
     */
    Scribe(int argc, const char *const argv[]);
    /**
     * @brief Emit field destruction locally.
     */
    ~Scribe();

public:
    /**
     * @brief Entry point of the application.
     *
     * @returns Exit status of the application (to be returned by @c main()).
     */
    int run();
    /**
     * @brief Retrieves global configuration object.
     *
     * @returns The configuration.
     */
    Config & getConfig();
    /**
     * @brief Retrieves path to location of all projects.
     *
     * @returns The path.
     */
    const std::string & getProjectsDir() const;
    /**
     * @brief Retrieves name of active project.
     *
     * @returns The name or empty string.
     */
    const std::string & getPrj() const;

private:
    /**
     * @brief Initializes arguments.
     *
     * @param argc Number of application arguments.
     * @param argv Application arguments.
     *
     * @throws std::runtime_error on broken argument list.
     */
    void initArgs(int argc, const char *const argv[]);
    /**
     * @brief Initializes configuration.
     */
    void initConfig();

private:
    /**
     * @brief Name of the project to operate on.
     */
    std::string prjName;
    /**
     * @brief Name of command to execute.
     */
    std::string cmdName;
    /**
     * @brief Arguments for the command specified by @c cmdName.
     */
    std::vector<std::string> args;
    /**
     * @brief Holds global configuration.
     */
    std::unique_ptr<Config> config;
    /**
     * @brief Root directory of all projects.
     */
    std::string projectsDir;
};

#endif // SCRIBE__SCRIBE_HPP__
