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

#include <string>
#include <vector>

#include "Catch/catch.hpp"

#include "parsing.hpp"

TEST_CASE("Assign-like pairs with invalid keys are ignored.",
          "[parsing][colon-style-pairs]")
{
    const std::vector<std::string> input = {
        "url:",
        "http://some.url/?show=#",
    };

    const std::vector<std::string> parsed = parsePairedArgs(input);

    REQUIRE(parsed.size() == 1);
    REQUIRE(parsed[0] == "url=http://some.url/?show=#");
}

TEST_CASE("No argument alias is expanded.", "[parsing][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd pre-arg";
        }
        return {};
    };

    std::vector<std::string> args = { "alias", "arg" };
    std::string cmd = parseInvocation(args, aliasResolver, false);

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "pre-arg");
    REQUIRE(args[1] == "arg");
}

TEST_CASE("Single argument alias is expanded.", "[parsing][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${1}";
        }
        return {};
    };

    std::vector<std::string> args = { "alias", "arg" };
    std::string cmd = parseInvocation(args, aliasResolver, false);

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 1);
    REQUIRE(args[0] == "arg");
}

TEST_CASE("Multiple argument alias is expanded.", "[parsing][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${1} ${2}";
        }
        return {};
    };

    std::vector<std::string> args = { "alias", "arg1", "arg2" };
    std::string cmd = parseInvocation(args, aliasResolver, false);

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "arg1");
    REQUIRE(args[1] == "arg2");
}

TEST_CASE("Unused arguments are appended.", "[parsing][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${2}";
        }
        return {};
    };

    std::vector<std::string> args = { "alias", "arg1", "arg2", "arg3" };
    std::string cmd = parseInvocation(args, aliasResolver, false);

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 3);
    REQUIRE(args[0] == "arg2");
    REQUIRE(args[1] == "arg1");
    REQUIRE(args[2] == "arg3");
}

TEST_CASE("Arguments can repeat.", "[parsing][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${1} ${1}";
        }
        return {};
    };

    std::vector<std::string> args = { "alias", "arg" };
    std::string cmd = parseInvocation(args, aliasResolver, false);

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "arg");
    REQUIRE(args[1] == "arg");
}

TEST_CASE("Invalid argument syntax is unchanged.", "[parsing][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${} ${-1} ${0} a${3}";
        }
        return {};
    };

    std::vector<std::string> args = { "alias" };
    std::string cmd = parseInvocation(args, aliasResolver, false);

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 4);
    REQUIRE(args[0] == "${}");
    REQUIRE(args[1] == "${-1}");
    REQUIRE(args[2] == "${0}");
    REQUIRE(args[3] == "a${3}");
}

TEST_CASE("Arguments can non-sequential.", "[parsing][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${2} ${1}";
        }
        return {};
    };

    std::vector<std::string> args = { "alias", "arg1", "arg2" };
    std::string cmd = parseInvocation(args, aliasResolver, false);

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "arg2");
    REQUIRE(args[1] == "arg1");
}

TEST_CASE("Absent arguments are expanded to empty strings.",
          "[parsing][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${10} ${11}";
        }
        return {};
    };

    std::vector<std::string> args = { "alias" };
    std::string cmd = parseInvocation(args, aliasResolver, false);

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "");
    REQUIRE(args[1] == "");
}

TEST_CASE("Completion stops after last argument is inserted.",
          "[parsing][aliases][completion]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${2} ${3} ${1}";
        }
        return {};
    };

    std::vector<std::string> args = { "alias", "arg1", "arg2", "arg3" };
    std::string cmd = parseInvocation(args, aliasResolver, true);

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "arg2");
    REQUIRE(args[1] == "arg3");
}
