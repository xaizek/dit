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
#include <utility>
#include <vector>

class Config;
class Project;

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
     * @brief Completes command-line.
     *
     * Always the last argument is completed.
     *
     * @param project Active project for completion.
     * @param args Arguments to complete.
     *
     * @returns Exit status of the application (to be returned by @c main()).
     */
    int complete(Project &project, std::vector<std::string> args);
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
    /**
     * @brief Makes standalone configuration for a project.
     *
     * @param path Path to real configuration file.
     *
     * @returns Pair of proxy (unsaved) and real configuration.
     */
    std::pair<Config, std::unique_ptr<Config>>
    makeConfig(const std::string &path) const;
    /**
     * @brief Maps alias name to its right-hand side.
     *
     * @param name Name of an alias.
     *
     * @returns Right-hand side of the alias.
     */
    std::string resolveAlias(const std::string &name) const;

private:
    /**
     * @brief Name of the project to operate on.
     */
    std::string prjName;
    /**
     * @brief Arguments passed to the application.
     *
     * Does not include project name and changes over time.
     */
    std::vector<std::string> args;
    /**
     * @brief Holds global configuration.
     */
    std::unique_ptr<Config> globalConfig;
    /**
     * @brief Root directory of all projects.
     */
    std::string projectsDir;
};

#endif // SCRIBE__SCRIBE_HPP__
