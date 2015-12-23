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

#include <cstdlib>

#include <string>
#include <unordered_set>
#include <vector>

#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "ItemFilter.hpp"
#include "ItemTable.hpp"
#include "Project.hpp"
#include "Storage.hpp"
#include "completion.hpp"
#include "integration.hpp"

/**
 * @brief Usage message for "ls" command.
 */
const char *const USAGE = R"(Usage: ls [expr...]

Where <expr> is of the form:

    <field> == <value>  --  case sensitive equality comparison
    <field> != <value>  --  case sensitive inequality comparison
    <field>  / <value>  --  case insensitive substring match
    <field> =/ <value>  --  case insensitive substring match
    <field>  # <value>  --  case insensitive substring non-match
    <field> !/ <value>  --  case insensitive substring non-match

For example:

    status==done title/ui
    category!=cli)";

namespace {

/**
 * @brief Implementation of "ls" command, which lists items.
 */
class LsCmd : public AutoRegisteredCommand<LsCmd>
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
    virtual boost::optional<int> run(
        Project &project,
        const std::vector<std::string> &args) override;
    /**
     * @copydoc Command::complete()
     */
    virtual boost::optional<int> complete(
        Project &project,
        const std::vector<std::string> &args) override;
};

}

LsCmd::LsCmd() : parent("ls", "lists items", USAGE)
{
}

boost::optional<int>
LsCmd::run(Project &project, const std::vector<std::string> &args)
{
    Config &config = project.getConfig();
    std::string fmt = config.get("ui.ls.fmt");
    std::string colorSpec = config.get("ui.ls.color");
    std::string sort = config.get("ui.ls.sort");

    ItemTable table(fmt, colorSpec, sort, getTerminalWidth());
    ItemFilter filter(args);

    for (Item &item : project.getStorage().list()) {
        if (filter.passes(item)) {
            table.append(item);
        }
    }

    table.print(out());

    return EXIT_SUCCESS;
}

boost::optional<int>
LsCmd::complete(Project &project, const std::vector<std::string> &)
{
    return completeKeys(project.getStorage(), out());
}
