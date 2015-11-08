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

#include "Invocation.hpp"

#include <functional>
#include <regex>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <boost/range/adaptor/reversed.hpp>
#include <boost/tokenizer.hpp>

#include "utils/containers.hpp"
#include "utils/strings.hpp"

std::vector<std::string> breakIntoArgs(const std::string &line);
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
                 return s.find('=') != std::string::npos;
             });

    confs.reserve(assigns.size());
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
    if (cmdLine.empty()) {
        cmdLine = breakIntoArgs(defCmdLine);
    }

    if (!cmdLine.empty()) {
        composition = cmdLine[0];
        cmdName = composition;
        cmdLine.erase(cmdLine.begin());
    }

    // break composition by a '.' and apply aliases from right to left, commands
    // that are not aliases are just prepended to current command-line
    namespace adaptors = boost::adaptors;
    const std::vector<std::string> names = split(composition, '.');
    for (const std::string &name : adaptors::reverse(names)) {
        const std::string rhsString = aliasResolver(name);
        if (rhsString.empty()) {
            cmdLine.insert(cmdLine.begin(), name);
            continue;
        }

        const std::vector<std::string> &rhs = breakIntoArgs(rhsString);
        setCmdLine(applyAlias(rhs, cmdLine, completion));
    }

    if (cmdLine.empty()) {
        cmdName.clear();
    } else {
        cmdName = cmdLine[0];
        cmdLine.erase(cmdLine.begin());
    }
}

/**
 * @brief Tokenize the command line, respecting escapes and quotes.
 *
 * @param line Line to parse.
 *
 * @returns Array of arguments.
 */
std::vector<std::string>
breakIntoArgs(const std::string &line)
{
    boost::escaped_list_separator<char> sep("\\", " ", "\"'");
    boost::tokenizer<boost::escaped_list_separator<char>> tok(line, sep);
    return std::vector<std::string>(tok.begin(), tok.end());
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
                    return std::move(substituted);
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

    return std::move(substituted);
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