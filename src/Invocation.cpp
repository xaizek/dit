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

#include "Invocation.hpp"

#include <algorithm>
#include <functional>
#include <regex>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <boost/program_options/errors.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/range/adaptor/reversed.hpp>

#include "utils/args.hpp"
#include "utils/containers.hpp"
#include "utils/contains.hpp"
#include "utils/strings.hpp"

namespace po = boost::program_options;

static std::vector<std::string> applyAlias(
    const std::vector<std::string> &alias, const std::vector<std::string> &args,
    bool completion);

void
Invocation::setCmdLine(std::vector<std::string> args)
{
    if (!args.empty() && !args[0].empty() && args[0].front() == '.') {
        prjName = args[0].substr(1);
        args.erase(args.begin());
    }

    std::vector<std::string> assigns;
    std::tie(assigns, cmdLine) =
        span(args,
             [](const std::string &s) {
                 return contains(s, '=');
             });

    confs.reserve(confs.size() + assigns.size());
    for (const std::string &assign : assigns) {
        confs.emplace_back(splitAt(assign, '='));
    }
}

void
Invocation::setDefCmdLine(std::string cmdLine)
{
    defCmdLine = std::move(cmdLine);
}

void
Invocation::setDefPrjName(std::string prjName)
{
    defPrjName = std::move(prjName);
}

void
Invocation::setAliasResolver(aliasResolverFunc resolver)
{
    aliasResolver = std::move(resolver);
}

void
Invocation::parse(bool completion)
{
    processOptions(completion);

    const bool takeComposition = !cmdLine.empty();
    if (takeComposition) {
        composition = cmdLine[0];
        cmdName = composition;
        cmdLine.erase(cmdLine.begin());
    }

    // Break composition by a '.' and apply aliases from right to left, commands
    // that are not aliases are just prepended to current command-line.
    namespace adaptors = boost::adaptors;
    std::vector<std::string> names = takeComposition && composition.empty()
                                   ? std::vector<std::string>({ std::string() })
                                   : split(composition, '.');
    if (std::count_if(names.cbegin(), names.cend(),
                      std::mem_fn(&std::string::empty)) > 1U) {
        names.assign({ composition });
    }

    auto processAlias = [this, completion](const std::string &name) {
        const std::string rhsString = aliasResolver(name);
        if (rhsString.empty()) {
            cmdLine.insert(cmdLine.begin(), name);
            return;
        }

        const std::vector<std::string> &rhs = breakIntoArgs(rhsString);
        setCmdLine(applyAlias(rhs, cmdLine, completion));
    };

    for (const std::string &name : adaptors::reverse(names)) {
        if (!name.empty()) {
            processAlias(name);
            continue;
        }

        if (defCmdLine.empty()) {
            continue;
        }

        std::vector<std::string> args = breakIntoArgs(defCmdLine);
        cmdLine.insert(cmdLine.cbegin(), args.cbegin(), args.cend());
        setCmdLine(cmdLine);

        if (!cmdLine.empty()) {
            std::vector<std::string> names = split(cmdLine.front(), '.');
            cmdLine.erase(cmdLine.begin());
            for (const std::string &name : adaptors::reverse(names)) {
                processAlias(name);
            }
        }
    }

    if (cmdLine.empty()) {
        cmdName.clear();
    } else {
        cmdName = cmdLine[0];
        cmdLine.erase(cmdLine.begin());
    }
}

void
Invocation::processOptions(bool completion)
{
    po::options_description opts("dit options");
    opts.add_options()
        ("help,h", "display help message")
        ("version,v", "display version information");
    std::ostringstream oss;
    oss << opts;
    helpMsg = oss.str();

    using opt_t = boost::shared_ptr<po::option_description>;
    optNames.clear();
    for (const opt_t &opt : opts.options()) {
        optNames.push_back("--" + opt->long_name());
    }
    optNames.push_back("-h");
    optNames.push_back("-v");

    po::options_description all;
    all.add(opts);

    std::vector<std::string> prefix;
    std::tie(prefix, cmdLine) = span(cmdLine, [](const std::string &a) {
                                         return !a.empty() && a.front() == '-';
                                     });

    po::variables_map vm;
    try {
        po::store(po::command_line_parser(prefix).options(all).run(), vm);
        po::notify(vm);
    } catch (const boost::program_options::error &) {
        // Allow unknown options for completion.
        if (!completion) {
            throw;
        }
    }

    help = vm.count("help");
    version = vm.count("version");

    if (cmdLine.empty()) {
        if (prefix.empty()) {
            cmdLine = breakIntoArgs(defCmdLine);
        } else {
            composition.clear();
            cmdName.clear();
        }
    }
}

/**
 * @brief Performs alias argument substitution.
 *
 * @param alias Right-hand side of the alias.
 * @param args Arguments passed to the alias.
 * @param completion Whether we're performing completion.
 *
 * @returns Expanded command-line.
 */
static std::vector<std::string>
applyAlias(const std::vector<std::string> &alias,
           const std::vector<std::string> &args, bool completion)
{
    // TODO: maybe display error to the user if not all alias arguments were
    //       supplied

    static std::regex argRegex(R"(\$\{(\d+)\})");

    std::vector<bool> argUsed(args.size());

    std::vector<std::string> substituted;

    std::smatch match;
    for (const std::string &arg : alias) {
        if (std::regex_match(arg, match, argRegex)) {
            const unsigned int argN = std::stoi(match[1].str());
            if (argN == 0) {
                substituted.push_back(arg);
            } else if (argN <= args.size()) {
                substituted.push_back(args[argN - 1U]);
                argUsed[argN - 1U] = true;

                // Stop after inserting last argument during completion.
                if (completion && argN == args.size()) {
                    return substituted;
                }
            } else {
                substituted.emplace_back();
            }
        } else {
            substituted.push_back(arg);
        }
    }

    // Append all unused elements.
    for (unsigned int i = 0; i < args.size(); ++i) {
        if (!argUsed[i]) {
            substituted.push_back(args[i]);
        }
    }

    return substituted;
}

bool
Invocation::shouldPrintHelp() const
{
    return help;
}

std::string
Invocation::getHelp() const
{
    return helpMsg;
}

std::vector<std::string>
Invocation::getOpts() const
{
    return optNames;
}

bool
Invocation::shouldPrintVersion() const
{
    return version;
}

std::string
Invocation::getPrjName() const
{
    return prjName.empty() ? defPrjName : prjName;
}

std::string
Invocation::getCmdName() const
{
    return cmdName;
}

std::string
Invocation::getComposition() const
{
    return composition;
}

std::vector<std::string>
Invocation::getCmdArgs() const
{
    return cmdLine;
}

std::vector<std::pair<std::string, std::string>>
Invocation::getConfs() const
{
    return confs;
}
