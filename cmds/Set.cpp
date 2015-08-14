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
#include <utility>

#include <boost/range/iterator_range.hpp>

#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"
#include "utils.hpp"

namespace {

/**
 * @brief Implementation of "set" command, which sets item information.
 */
class SetCmd : public Command
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    SetCmd();

public:
    /**
     * @copydoc Command::run()
     */
    virtual int run(Project &project,
                    const std::vector<std::string> &args) override;
};

REGISTER_COMMAND(SetCmd);

}

SetCmd::SetCmd() : Command("set", "changes items", "Usage: set id key=value...")
{
}

int
SetCmd::run(Project &project, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        std::cerr << "set: Expected at least two arguments." << std::endl;
        return EXIT_FAILURE;
    }

    const std::string &id = args[0];
    Item &item = project.getStorage().get(id);

    for (const std::string &a : boost::make_iterator_range(args.begin() + 1,
                                                           args.end())) {
        std::string key, value;
        std::tie(key, value) = splitAt(a, '=');
        item.setValue(key, value);
    }

    return EXIT_SUCCESS;
}
