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

#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "utils/strings.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"

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
    virtual boost::optional<int> run(
        Project &project,
        const std::vector<std::string> &args) override;
};

REGISTER_COMMAND(AddCmd);

}

AddCmd::AddCmd() : Command("add", "add new item", "Usage: add key=value")
{
}

boost::optional<int>
AddCmd::run(Project &project, const std::vector<std::string> &args)
{
    if (args.empty()) {
        err() << "Expected at least one argument.\n";
        return EXIT_FAILURE;
    }

    Item &item = project.getStorage().create();

    for (const std::string &a : args) {
        std::string key, value;
        std::tie(key, value) = splitAt(a, '=');

        std::string error;
        if (!Item::isValidKeyName(key, true, error)) {
            out() << "Wrong key name \"" << key << "\": " << error << '\n';
            return EXIT_FAILURE;
        }

        item.setValue(key, value);
    }

    out() << "Created item: " << item.getId() << '\n';

    return EXIT_SUCCESS;
}

bool
operator<(const Command &l, const Command &r)
{
    return l.getName() < r.getName();
}
