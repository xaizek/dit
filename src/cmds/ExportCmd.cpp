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

#include <unistd.h>

#include <cstdlib>

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/find_format.hpp>
#include <boost/algorithm/string/finder.hpp>

#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "ItemFilter.hpp"
#include "Project.hpp"
#include "completion.hpp"

/**
 * @brief Usage message for "export" command.
 */
const char *const USAGE = R"(Usage: export (-|cmd) [expr like for ls...]

Either the cmd is run once for each item with arguments of the form key=value
or items are printed to standard output with key=value fields terminated by null
character and each item also finished by null character.)";

namespace {

/**
 * @brief Implementation of "export" command, which passes item data to command.
 */
class ExportCmd : public AutoRegisteredCommand<ExportCmd>
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    ExportCmd();

public:
    /**
     * @copydoc Command::run()
     *
     * @throws std::runtime_error On external command failure.
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
     * @brief Invokes the command for given item.
     *
     * @param cmd Beginning of command line to be constructed.
     * @param item Source of item data.
     *
     * @throws std::runtime_error On external command failure.
     */
    void exportItem(const std::string &cmd, Item &item);
    /**
     * @brief Escapes special shell characters in a string.
     *
     * @param str String to escape.
     *
     * @returns Escaped string.
     */
    std::string shellEscape(std::string str);
};

/**
 * @brief Command-line escaping helper.
 */
class ShellEscaper
{
public:
    /**
     * @brief Escapes range of characters.
     *
     * @tparam T Type of range.
     * @param match Range itself.
     *
     * @returns Escaped range.
     */
    template <typename T>
    std::string operator()(const T& match) const
    {
        std::ostringstream oss;
        for (const auto &c : match) {
            oss << '\\' << c;
        }
        return oss.str();
    }
};

}

ExportCmd::ExportCmd() : parent("export", "item data exporter", USAGE)
{
}

boost::optional<int>
ExportCmd::run(Project &project, const std::vector<std::string> &args)
{
    if (args.empty()) {
        err() << "Expected at least one argument.\n";
        return EXIT_FAILURE;
    }

    const std::string &cmd = args.front();
    ItemFilter filter({ args.cbegin() + 1, args.cend() });

    for (Item &item : project.getStorage().list()) {
        if (filter.passes(item)) {
            exportItem(cmd, item);
        }
    }

    return EXIT_SUCCESS;
}

boost::optional<int>
ExportCmd::complete(Project &project, const std::vector<std::string> &args)
{
    if (args.size() <= 1U) {
        return EXIT_SUCCESS;
    }
    return completeKeys(project.getStorage(), out());
}

void
ExportCmd::exportItem(const std::string &cmd, Item &item)
{
    // Check for special value meaning "printing to stdout".
    if (cmd == "-") {
        out() << "_id=" << item.getValue("_id") << '\0';
        for (const std::string &key : item.listRecordNames()) {
            out() << key << '=' << item.getValue(key) << '\0';
        }
        out() << '\0';
        return;
    }

    // Compose command-line.
    std::ostringstream cmdLine(cmd, std::ios::out | std::ios::ate);
    cmdLine << " _id=" << shellEscape(item.getValue("_id"));
    for (const std::string &key : item.listRecordNames()) {
        cmdLine << ' ' << key << '=' << shellEscape(item.getValue(key));
    }

    // Execute the command-line.
    int status = std::system(cmdLine.str().c_str());
    bool success = (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS);

    // Throw an exception on failure.
    if (!success) {
        throw std::runtime_error("Exporter client returned an error.");
    }
}

std::string
ExportCmd::shellEscape(std::string str)
{
    // Turn newlines into spaces and let export client wrap the result if
    // needed.
    // XXX: might not be the best solution, but will do for most use cases.
    std::replace(str.begin(), str.end(), '\n', ' ');

    // Escape characters that have special meaning for shells.
    boost::find_format_all(
        str,
        boost::token_finder(
            boost::is_any_of("\\\r\n\t'\"; ?|[]{}<>`!$&*()#~")),
        ShellEscaper());

    return str;
}
