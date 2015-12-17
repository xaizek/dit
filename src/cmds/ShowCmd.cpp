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

#include <algorithm>
#include <ostream>
#include <string>
#include <vector>

#include "utils/strings.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"
#include "decoration.hpp"

template <typename C>
static inline bool contains(const C &c, const typename C::value_type &what);

namespace {

/**
 * @brief Implementation of "show" command, which displays item information.
 */
class ShowCmd : public AutoRegisteredCommand<ShowCmd>
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    ShowCmd();

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

private:
    /**
     * @brief Prints out single record.
     *
     * @param name Name of the record (e.g. "type").
     * @param val Value of the record (e.g. "bug").
     */
    void printRecord(const std::string &name, const std::string &val);
};

}

ShowCmd::ShowCmd() : parent("show", "displays items", "Usage: show id")
{
}

boost::optional<int>
ShowCmd::run(Project &project, const std::vector<std::string> &args)
{
    if (args.size() != 1) {
        err() << "Expected single argument (id).\n";
        return EXIT_FAILURE;
    }

    Config &config = project.getConfig();
    const std::string order = config.get("ui.show.order");

    const std::vector<std::string> ordering = split(order, ',');

    const std::string &id = args[0];
    Item &item = project.getStorage().get(id);

    // Print known fields in specified order first.
    for (const std::string &field : ordering) {
        const std::string &v = item.getValue(field);
        if (!v.empty()) {
            printRecord(field, v);
        }
    }

    // Print out the rest of the fields.
    for (const std::string &c : item.listRecordNames()) {
        if (!contains(ordering, c)) {
            printRecord(c, item.getValue(c));;
        }
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Checks whether container contains given item.
 *
 * @tparam C Type of the container.
 * @param c Container to examine.
 * @param what Item to look up in the container.
 *
 * @returns @c true when item is found, @c false otherwise.
 */
template <typename C>
static inline bool
contains(const C &c, const typename C::value_type &what)
{
    return std::find(c.cbegin(), c.cend(), what) != c.cend();
}

void
ShowCmd::printRecord(const std::string &name, const std::string &val)
{
    out() << decor::bold << name << decor::def
          << (val.find('\n') == std::string::npos ? ": " : ":\n")
          << val<< '\n';
}

boost::optional<int>
ShowCmd::complete(Project &project, const std::vector<std::string> &args)
{
    if (args.size() > 1) {
        return EXIT_FAILURE;
    }

    for (Item &item : project.getStorage().list()) {
        out() << item.getId() << '\n';
    }

    return EXIT_SUCCESS;
}
