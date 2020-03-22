// Copyright (C) 2015 xaizek <xaizek@posteo.net>
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

#include <map>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <boost/range/iterator_range.hpp>

#include "utils/contains.hpp"
#include "utils/strings.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"
#include "completion.hpp"
#include "integration.hpp"
#include "parsing.hpp"

/**
 * @brief Usage message for "set" command.
 */
const char *const USAGE = R"(Usage: set id key[+]=value...

Sets or appends to values of items:

    key=value   --  set new value
    key=-       --  set/edit value via external editor
    key+=value  --  append to the old value after new-line character
    key+=-      --  append value via external editor

For example:

    status=done comment+='This was a hard one.')";

namespace {

/**
 * @brief Implementation of "set" command, which sets item information.
 */
class SetCmd : public AutoRegisteredCommand<SetCmd>
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

}

SetCmd::SetCmd() : parent("set", "modify item entries", USAGE)
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

    std::map<std::string, std::string> fields;

    for (const std::string &a : parsePairedArgs({ args.cbegin() + 1,
                                                  args.cend() })) {
        std::string key, value;
        std::tie(key, value) = splitAt(a, '=');

        bool append = false;
        if (!key.empty() && key.back() == '+') {
            append = true;
            key.pop_back();
        }

        std::string error;
        if (!Item::isValidKeyName(key, true, error)) {
            err() << "Wrong key name \"" << key << "\": " << error << '\n';
            return EXIT_FAILURE;
        }

        if (!contains(fields, key)) {
            fields[key] = item.getValue(key);
        }

        const std::string current = append ? std::string() : fields[key];
        if (value == "-") {
            boost::optional<std::string> v = editValue(key, current);
            if (!v) {
                err() << "Failed to prompt for value of key \""
                      << key << "\"\n";
                return EXIT_FAILURE;
            }
            value = std::move(*v);
        }

        if (append) {
            std::string current = fields[key];
            if (!current.empty()) {
                // XXX: should we add new line if `value` is empty?
                current += '\n';
            }
            fields[key] = current + value;
        } else {
            fields[key] = value;
        }
    }

    for (auto field : fields) {
        item.setValue(field.first, field.second);
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

    // Complete values.
    std::vector<std::string> parsedArgs = parsePairedArgs(args);
    if (!args.empty() && contains(parsedArgs.back(), '=')) {
        std::string key, value;
        std::tie(key, value) = splitAt(parsedArgs.back(), '=');

        if (value.empty() || value == args.back()) {
            return completeValues(storage, out(), key);
        }
    }

    return completeKeys(storage, out(), { args.cbegin() + 1, args.cend() });
}
