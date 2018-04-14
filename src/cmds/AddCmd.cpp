// Copyright (C) 2015 xaizek <xaizek@posteo.net>
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

#include <ostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "utils/args.hpp"
#include "utils/contains.hpp"
#include "utils/strings.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "ItemFilter.hpp"
#include "Project.hpp"
#include "Storage.hpp"
#include "completion.hpp"
#include "integration.hpp"
#include "parsing.hpp"

/**
 * @brief Usage message for "add" command.
 */
const char *const USAGE = R"(Usage: add key=value...

Sets or appends to values of items:

    key=value   --  set new value
    key=-       --  set new value via external editor
    key+=value  --  append value (from earlier key=value pair)
    key+=-      --  append value via external editor)";

namespace {

/**
 * @brief Implementation of "add" command, which sets item information.
 */
class AddCmd : public AutoRegisteredCommand<AddCmd>
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
    /**
     * @copydoc Command::complete()
     */
    virtual boost::optional<int> complete(
        Project &project,
        const std::vector<std::string> &args) override;
};

}

AddCmd::AddCmd() : parent("add", "add new item", USAGE)
{
}

boost::optional<int>
AddCmd::run(Project &project, const std::vector<std::string> &args)
{
    if (args.empty()) {
        err() << "Expected at least one argument.\n";
        return EXIT_FAILURE;
    }

    std::unordered_map<std::string, std::string> fields;

    for (const std::string &a : parsePairedArgs(args)) {
        std::string key, value;
        std::tie(key, value) = splitAt(a, '=');

        std::string error;
        if (!Item::isValidKeyName(key, true, error)) {
            err() << "Wrong key name \"" << key << "\": " << error << '\n';
            return EXIT_FAILURE;
        }

        if (boost::optional<std::string> v = editValue(key, value, {})) {
            value = std::move(*v);
        }
        fields[key] = value;
    }

    Config &cfg = project.getConfig();

    for (const std::string &defKey : cfg.list("defaults")) {
        if (!contains(fields, defKey)) {
            fields[defKey] = cfg.get("defaults." + defKey);
        }
    }

    std::string guard = cfg.get("guards.newitem", std::string());
    auto accessor = [&fields](const std::string &f) {
        auto it = fields.find(f);
        return (it == fields.cend()) ? std::string() : it->second;
    };
    std::string error;
    if (!ItemFilter(breakIntoArgs(guard)).passes(accessor, error)) {
        err() << "New item doesn't pass the guard: " << guard << '\n'
              << error << '\n';
        return EXIT_FAILURE;
    }

    Item &item = project.getStorage().create();
    for (const auto &entry : fields) {
        item.setValue(entry.first, entry.second);
    }
    out() << "Created item: " << item.getId() << '\n';

    return EXIT_SUCCESS;
}

boost::optional<int>
AddCmd::complete(Project &project, const std::vector<std::string> &args)
{
    Storage &storage = project.getStorage();

    std::vector<std::string> parsedArgs = parsePairedArgs(args);

    if (!args.empty() && contains(parsedArgs.back(), '=')) {
        std::string key, value;
        std::tie(key, value) = splitAt(parsedArgs.back(), '=');

        if (value.empty() || value == args.back()) {
            return completeValues(storage, out(), key);
        }
    }

    return completeKeys(storage, out(), args);
}
