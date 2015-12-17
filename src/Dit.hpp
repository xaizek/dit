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

#ifndef DIT__DIT_HPP__
#define DIT__DIT_HPP__

#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Invocation.hpp"

class Config;
class Project;

/**
 * @brief Application implementation.
 */
class Dit
{
public:
    /**
     * @brief Constructs main application class.
     *
     * @param args Application arguments.
     *
     * @throws std::runtime_error on broken argument list.
     */
    Dit(std::vector<std::string> args);
    /**
     * @brief Emit field destruction locally.
     */
    ~Dit();

public:
    /**
     * @brief Entry point of the application.
     *
     * @returns Exit status of the application (to be returned by @c main()).
     */
    int run();
    /**
     * @brief Performs command-line completion.
     *
     * @param args Arguments to complete.
     * @param out Output stream.
     * @param err Error stream.
     *
     * @returns Exit status of the application (to be returned by @c main()).
     */
    int complete(std::vector<std::string> args, std::ostream &out,
                 std::ostream &err);
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
    std::string getPrj() const;

private:
    /**
     * @brief Initializes configuration.
     */
    void initConfig();
    /**
     * @brief Completes command-line for a sub-command.
     *
     * Uses current state of @c invocation.  Always the last argument is
     * completed.
     *
     * @returns Exit status of the application (to be returned by @c main()).
     */
    int completeCmd();
    /**
     * @brief Looks up a project by its name.
     *
     * @param name Name of the project to look up.
     * @param error Error message on failed look up.
     *
     * @returns The project or nothing setting @p error to a message.
     */
    std::unique_ptr<Project> openProject(const std::string &name,
                                         std::string &error);
    /**
     * @brief Makes standalone configuration for a project.
     *
     * @param path Path to real configuration file.
     *
     * @returns Pair of proxy (unsaved) and real configuration.
     */
    std::pair<Config, std::unique_ptr<Config>>
    makeConfig(const std::string &path) const;

private:
    /**
     * @brief Parses command-line into components.
     */
    Invocation invocation;
    /**
     * @brief Holds global configuration.
     */
    std::unique_ptr<Config> globalConfig;
    /**
     * @brief Root directory of all projects.
     */
    std::string projectsDir;
};

#endif // DIT__DIT_HPP__
