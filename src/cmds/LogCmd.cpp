// Copyright (C) 2016 xaizek <xaizek@openmailbox.org>
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

#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Change.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"
#include "completion.hpp"
#include "decoration.hpp"
#include "printing.hpp"

namespace {

/**
 * @brief Implementation of "log" command, which displays changes of items.
 */
class LogCmd : public AutoRegisteredCommand<LogCmd>
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    LogCmd();

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

LogCmd::LogCmd() : parent("log", "display item changes",
                          "Usage: log id [key...]")
{
}

boost::optional<int>
LogCmd::run(Project &project, const std::vector<std::string> &args)
{
    if (args.size() < 1) {
        err() << "Expected single argument (id).\n";
        return EXIT_FAILURE;
    }

    const std::string &id = args[0];
    std::unordered_set<std::string> filter(++args.cbegin(), args.end());

    Item &item = project.getStorage().get(id);
    const std::vector<Change> &changes = item.getChanges();

    std::unordered_map<std::string, std::string> values;

    for (const Change &change : changes) {
        const std::string &key = change.getKey();
        const std::string &value = change.getValue();

        if (!filter.empty() && filter.find(key) == filter.end()) {
            continue;
        }

        if (value.empty()) {
            out() << Key{key}
                  << decor::red_fg << decor::bold << " deleted\n" << decor::def;
        } else if (values[key].empty()) {
            out() << Key{key}
                  << decor::yellow_fg << decor::bold << " created" << decor::def
                  << Value{value} << '\n';
        } else {
            out() << Key{key}
                  << decor::blue_fg << decor::bold << " changed to"
                  << decor::def
                  << Value{value} << '\n';
        }

        values[key] = value;
    }

    return EXIT_SUCCESS;
}

boost::optional<int>
LogCmd::complete(Project &project, const std::vector<std::string> &args)
{
    if (args.size() <= 1) {
        return completeIds(project.getStorage(), out());
    }

    try {
        const std::string &id = args[0];
        Item &item = project.getStorage().get(id);
        return completeKeys(item, out(), { ++args.cbegin(), args.cend() });
    } catch (std::runtime_error &) {
        return EXIT_FAILURE;
    }
}
