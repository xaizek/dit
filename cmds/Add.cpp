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
#include <utility>
#include <vector>

#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"
#include "utils.hpp"

namespace {

/**
 * @brief Implementation of "add" command, which sets item information.
 */
class AddCmd : public Command
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    AddCmd();

public:
    /**
     * @copydoc Command::run()
     */
    virtual int run(Project &project,
                    const std::vector<std::string> &args) override;
};

REGISTER_COMMAND(AddCmd);

}

AddCmd::AddCmd() : Command("add", "add new item", "Usage: add key=value")
{
}

int
AddCmd::run(Project &project, const std::vector<std::string> &args)
{
    if (args.empty()) {
        std::cerr << "help: Expected at least one argument." << std::endl;
        return EXIT_FAILURE;
    }

    Item &item = project.getStorage().create();

    for (const std::string &a : args) {
        std::string key, value;
        std::tie(key, value) = splitAt(a, '=');
        item.setValue(key, value);
    }

    std::cout << "Created item: " << item.getId() << std::endl;

    return EXIT_SUCCESS;
}

bool
operator<(const Command &l, const Command &r)
{
    return l.getName() < r.getName();
}
