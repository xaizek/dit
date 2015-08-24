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
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include <boost/range/iterator_range.hpp>

#include "utils/strings.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"

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

REGISTER_COMMAND(SetCmd);

}

SetCmd::SetCmd() : Command("set", "changes items", "Usage: set id key=value...")
{
}

boost::optional<int>
SetCmd::run(Project &project, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        err() << "Expected at least two arguments.\n";
        return EXIT_FAILURE;
    }

    const std::string &id = args[0];
    Item &item = project.getStorage().get(id);

    for (const std::string &a : boost::make_iterator_range(args.begin() + 1,
                                                           args.end())) {
        std::string key, value;
        std::tie(key, value) = splitAt(a, '=');

        std::string error;
        if (!Item::isValidKeyName(key, true, error)) {
            out() << "Wrong key name \"" << key << "\": " << error << '\n';
            return EXIT_FAILURE;
        }

        item.setValue(key, value);
    }

    return EXIT_SUCCESS;
}

boost::optional<int>
SetCmd::complete(Project &project, const std::vector<std::string> &args)
{
    Storage &storage = project.getStorage();

    // Complete item id.
    if (args.size() <= 1) {
        for (Item &item : storage.list()) {
            out() << item.getId() << '\n';
        }
        return EXIT_SUCCESS;
    }

    // Complete value.
    if (args.back().find('=') != std::string::npos) {
        std::string key = args.back();
        key.pop_back();

        try {
            Item &item = storage.get(args[0]);
            out() << "'" << item.getValue(key) << "'\n";
            return EXIT_SUCCESS;
        } catch (std::runtime_error &) {
            return EXIT_FAILURE;
        }
    }

    // Complete key.

    std::unordered_set<std::string> keys;

    for (Item &item : storage.list()) {
        const std::set<std::string> &itemKeys = item.listRecordNames();
        keys.insert(itemKeys.cbegin(), itemKeys.cend());
    }

    for (const std::string &key : keys) {
        out() << key << '\n';
    }

    return EXIT_SUCCESS;
}
