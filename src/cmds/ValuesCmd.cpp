// Copyright (C) 2018 xaizek <xaizek@posteo.net>
//
// This file is part of dit.
//
// dit is free software: you can redistribute it and/or modify
// it under the terms of version 3 of the GNU Affero General Public
// License as published by the Free Software Foundation.
//
// dit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dit.  If not, see <http://www.gnu.org/licenses/>.

#include <cstdlib>

#include <algorithm>
#include <ostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "utils/contains.hpp"
#include "utils/strings.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"
#include "completion.hpp"
#include "printing.hpp"

namespace {

/**
 * @brief Implementation of "values" command, which displays item information.
 */
class ValuesCmd : public AutoRegisteredCommand<ValuesCmd>
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    ValuesCmd();

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

ValuesCmd::ValuesCmd() : parent("values", "display all values of a key",
                                "Usage: values key")
{
}

boost::optional<int>
ValuesCmd::run(Project &project, const std::vector<std::string> &args)
{
    if (args.size() != 1U) {
        err() << "Expected at least one argument (id).\n";
        return EXIT_FAILURE;
    }

    std::set<std::string> values;

    for (Item &item : project.getStorage().list()) {
        std::string value = item.getValue(args[0]);
        if (!value.empty()) {
            values.insert(std::move(value));
        }
    }

    for (const std::string &value : values) {
        out() << value << '\n';
    }

    return EXIT_SUCCESS;
}

boost::optional<int>
ValuesCmd::complete(Project &project, const std::vector<std::string> &args)
{
    if (args.size() > 1U) {
        return EXIT_FAILURE;
    }

    return completeKeys(project.getStorage(), out());
}
