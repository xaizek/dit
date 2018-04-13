// Copyright (C) 2016 xaizek <xaizek@posteo.net>
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

#include <boost/multi_array.hpp>
#include <boost/program_options.hpp>

#include <cstdlib>

#include <deque>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "utils/contains.hpp"
#include "utils/opts.hpp"
#include "utils/strings.hpp"
#include "utils/time.hpp"
#include "Change.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"
#include "completion.hpp"
#include "decoration.hpp"
#include "printing.hpp"

namespace po = boost::program_options;

static std::string diff(const std::vector<std::string> &f,
                        const std::vector<std::string> &s);

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

private:
    /**
     * @brief Options of the sub-command.
     */
    po::options_description opts;
};

}

LogCmd::LogCmd()
    : parent("log", "display item changes",
             "Usage: log [--help|-h] [--timestamps|-t] id [key...]"),
      opts("log sub-command options")
{
    opts.add_options()
        ("help,h", "display help message")
        ("timestamps,t", "display when changes happened");
}

boost::optional<int>
LogCmd::run(Project &project, const std::vector<std::string> &args)
{
    po::variables_map vm = parseOpts(args, opts);
    if (vm.count("help")) {
        out() << opts;
        return EXIT_SUCCESS;
    }

    if (vm.count("positional") < 1U) {
        err() << "Expected at least one argument (id).\n";
        return EXIT_FAILURE;
    }

    const bool withTimestamps = vm.count("timestamps");

    auto positional = vm["positional"].as<std::vector<std::string>>();

    const std::string &id = positional[0];
    std::unordered_set<std::string> filter {
        ++positional.cbegin(), positional.cend()
    };

    Item &item = project.getStorage().get(id);
    const std::vector<Change> &changes = item.getChanges();

    std::unordered_map<std::string, std::string> values;

    for (const Change &change : changes) {
        const std::string &key = change.getKey();
        const std::string &value = change.getValue();

        if (!filter.empty() && !contains(filter, key)) {
            continue;
        }

        const std::string at = withTimestamps
                             ? " (" + timeToString(change.getTimestamp()) + ')'
                             : std::string();
        if (value.empty()) {
            out() << Key{key} << (decor::red_fg + decor::bold << " deleted")
                  << at << '\n';
        } else if (values[key].empty()) {
            out() << Key{key} << (decor::yellow_fg + decor::bold << " created")
                  << at
                  << Value{value} << '\n';
        } else {
            out() << Key{key} << (decor::blue_fg + decor::bold << " changed")
                  << at
                  << Value{diff(split(values[key], '\n'), split(value, '\n'))};
        }

        values[key] = value;
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Finds difference between two lists of lines.
 *
 * Implements solution for longest common subsequence problem that matches
 * modified finding of edit distance (substitution operation excluded) with
 * backtracking afterward to compose result.
 *
 * @param f New state.
 * @param s Previous state.
 *
 * @returns Colored difference showing how to get new state from the old one.
 */
static std::string
diff(const std::vector<std::string> &f, const std::vector<std::string> &s)
{
    boost::multi_array<int, 2> d(boost::extents[f.size() + 1][s.size() + 1]);

    // Modified edit distance finding.
    using size_type = std::vector<std::string>::size_type;
    for (size_type i = 0U, nf = f.size(); i <= nf; ++i) {
        for (size_type j = 0U, ns = s.size(); j <= ns; ++j) {
            if (i == 0U) {
                d[i][j] = j;
            } else if (j == 0U) {
                d[i][j] = i;
            } else {
                d[i][j] = std::min(d[i - 1U][j] + 1, d[i][j - 1U] + 1);
                if (f[i - 1U] == s[j - 1U]) {
                    d[i][j] = std::min(d[i - 1U][j - 1U], d[i][j]);
                }
            }
        }
    }

    std::deque<std::string> result;
    int identicalLines = 0;

    auto foldIdentical = [&identicalLines, &result]() {
        if (identicalLines > 3) {
            result.erase(result.cbegin() + 1,
                         result.cbegin() + (identicalLines - 1));
            result.insert(result.cbegin() + 1,
                          "<" + std::to_string(identicalLines - 2) +
                          " unchanged lines folded>");
        }
        identicalLines = 0;
    };

    // Compose results with folding of long runs of identical lines (longer than
    // three lines).
    int i = f.size(), j = s.size();
    while (i != 0 || j != 0) {
        if (i == 0) {
            foldIdentical();
            result.push_front("+ " + s[--j]);
        } else if (j == 0) {
            foldIdentical();
            result.push_front("- " + f[--i]);
        } else if (d[i][j] == d[i][j - 1] + 1) {
            foldIdentical();
            result.push_front("+ " + s[--j]);
        } else if (d[i][j] == d[i - 1][j] + 1) {
            foldIdentical();
            result.push_front("- " + f[--i]);
        } else {
            result.push_front("  " + f[--i]);
            --j;
            ++identicalLines;
        }
    }
    foldIdentical();

    // Color results and turn them into a string.
    std::ostringstream oss;
    for (const std::string &line : result) {
        decor::Decoration dec;
        switch (line[0]) {
            case '+': dec = decor::green_fg; break;
            case '-': dec = decor::red_fg;   break;
            case '<': dec = decor::black_fg + decor::bold; break;
        }
        oss << (dec << line) << '\n';
    }

    return oss.str();
}

boost::optional<int>
LogCmd::complete(Project &project, const std::vector<std::string> &args)
{
    using opt_t = boost::shared_ptr<po::option_description>;
    for (const opt_t &opt : opts.options()) {
        out() << "--" << opt->long_name() << '\n';
    }
    out() << "-h\n" << "-t\n";

    if (args.size() <= 1U) {
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
