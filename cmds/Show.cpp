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
 * @brief Implementation of "show" command, which displays item information.
 */
class Show : public Command
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    Show();

public:
    virtual int run(Project &project,
                    const std::vector<std::string> &args) override;
};

REGISTER_COMMAND(Show);

}

Show::Show() : Command("show", "displays items", "Usage: show id")
{
}

int
Show::run(Project &project, const std::vector<std::string> &args)
{
    if (args.size() != 1) {
        std::cerr << "show: Expected single argument (id)." << std::endl;
        return EXIT_FAILURE;
    }

    const std::string &id = args[0];
    Item &item = project.getStorage().get(id);

    for (const std::string &c : item.listRecordNames()) {
        const std::string &v = item.getValue(c);
        std::cout << c << " = " << v << std::endl;
    }

    return EXIT_SUCCESS;
}
