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

#include <string>
#include <vector>

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

public:
    /**
     * @brief Entry point of the application.
     *
     * @returns Exit status of the applicatio (to be returned by @c main()).
     */
    int run();

private:
    /**
     * @brief Name used to run the application.
     */
    std::string appName;
    /**
     * @brief Name of command to execute.
     */
    std::string cmdName;
    /**
     * @brief Arguments for the command specified by @c cmdName.
     */
    std::vector<std::string> args;
};

#endif // SCRIBE__SCRIBE_HPP__
