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

#include <cstdlib>

#include <iostream>
#include <string>
#include <vector>

#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"

namespace {

/**
 * @brief Implementation of "ls" command, which lists items.
 */
class LsCmd : public Command
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    LsCmd();

public:
    /**
     * @copydoc Command::run()
     */
    virtual int run(Project &project,
                    const std::vector<std::string> &args) override;
};

REGISTER_COMMAND(LsCmd);

}

LsCmd::LsCmd() : Command("ls", "lists items", "Usage: ls")
{
}

int
LsCmd::run(Project &project, const std::vector<std::string> &args)
{
    static_cast<void>(args);

    for (Item &item : project.getStorage().list()) {
        std::cout << item.getId() << std::endl;
    }

    return EXIT_SUCCESS;
}
