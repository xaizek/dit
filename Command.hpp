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

#ifndef SCRIBE__COMMAND_HPP__
#define SCRIBE__COMMAND_HPP__

#include <string>
#include <vector>

#include <boost/optional.hpp>

class Project;
class Scribe;

/**
 * @brief Base class for sub-commands.
 */
class Command
{
protected:
    /**
     * @brief Constructs sub-command.
     *
     * @param name Name that will be checked against and displayed.
     * @param descr Short description.
     * @param help Long description.
     */
    Command(std::string name, std::string descr, std::string help);

public:
    /**
     * @brief Properly destructs objects of derived classes.
     */
    virtual ~Command() = default;

public:
    /**
     * @brief Gets name of the command to match against and to display.
     *
     * @returns The name.
     */
    std::string getName() const;
    /**
     * @brief Gets short description of the command for lists.
     *
     * @returns The description.
     */
    std::string getDescr() const;
    /**
     * @brief Gets long description of the command for detailed help.
     *
     * @returns The description.
     */
    std::string getHelp() const;

public:
    /**
     * @brief Executes generic command (no project) with specified arguments.
     *
     * @param scribe Application instance.
     * @param args List of arguments for the command.
     *
     * @returns Exit code suitable for returning it from @c main() or empty
     *          value if not implemented.
     */
    virtual boost::optional<int> run(Scribe &scribe,
                                     const std::vector<std::string> &args);
    /**
     * @brief Executes project command with specified arguments.
     *
     * @param project Project to perform operation on.
     * @param args List of arguments for the command.
     *
     * @returns Exit code suitable for returning it from @c main() or empty
     *          value if not implemented.
     */
    virtual boost::optional<int> run(Project &project,
                                     const std::vector<std::string> &args);

private:
    /**
     * @brief Name of the command.
     */
    const std::string name;
    /**
     * @brief Short description.
     */
    const std::string descr;
    /**
     * @brief Longer description.
     */
    const std::string help;
};

#endif // SCRIBE__COMMAND_HPP__
