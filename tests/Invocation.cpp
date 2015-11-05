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

#include "Invocation.hpp"

TEST_CASE("No argument alias is expanded.", "[invocation][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd pre-arg";
        }
        return {};
    };

    Invocation invocation;
    invocation.setCmdLine({ "alias", "arg" });
    invocation.setAliasResolver(aliasResolver);
    invocation.parse();

    std::string cmd = invocation.getCmdName();
    std::vector<std::string> args = invocation.getCmdArgs();

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "pre-arg");
    REQUIRE(args[1] == "arg");
}

TEST_CASE("Single argument alias is expanded.", "[invocation][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${1}";
        }
        return {};
    };

    Invocation invocation;
    invocation.setCmdLine({ "alias", "arg" });
    invocation.setAliasResolver(aliasResolver);
    invocation.parse();

    std::string cmd = invocation.getCmdName();
    std::vector<std::string> args = invocation.getCmdArgs();

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 1);
    REQUIRE(args[0] == "arg");
}

TEST_CASE("Multiple argument alias is expanded.", "[invocation][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${1} ${2}";
        }
        return {};
    };

    Invocation invocation;
    invocation.setCmdLine({ "alias", "arg1", "arg2" });
    invocation.setAliasResolver(aliasResolver);
    invocation.parse();

    std::string cmd = invocation.getCmdName();
    std::vector<std::string> args = invocation.getCmdArgs();

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "arg1");
    REQUIRE(args[1] == "arg2");
}

TEST_CASE("Unused arguments are appended.", "[invocation][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${2}";
        }
        return {};
    };

    Invocation invocation;
    invocation.setCmdLine({ "alias", "arg1", "arg2", "arg3" });
    invocation.setAliasResolver(aliasResolver);
    invocation.parse();

    std::string cmd = invocation.getCmdName();
    std::vector<std::string> args = invocation.getCmdArgs();

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 3);
    REQUIRE(args[0] == "arg2");
    REQUIRE(args[1] == "arg1");
    REQUIRE(args[2] == "arg3");
}

TEST_CASE("Arguments can repeat.", "[invocation][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${1} ${1}";
        }
        return {};
    };

    Invocation invocation;
    invocation.setCmdLine({ "alias", "arg" });
    invocation.setAliasResolver(aliasResolver);
    invocation.parse();

    std::string cmd = invocation.getCmdName();
    std::vector<std::string> args = invocation.getCmdArgs();

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "arg");
    REQUIRE(args[1] == "arg");
}

TEST_CASE("Invalid argument syntax is unchanged.", "[invocation][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${} ${-1} ${0} a${3}";
        }
        return {};
    };

    Invocation invocation;
    invocation.setCmdLine({ "alias" });
    invocation.setAliasResolver(aliasResolver);
    invocation.parse();

    std::string cmd = invocation.getCmdName();
    std::vector<std::string> args = invocation.getCmdArgs();

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 4);
    REQUIRE(args[0] == "${}");
    REQUIRE(args[1] == "${-1}");
    REQUIRE(args[2] == "${0}");
    REQUIRE(args[3] == "a${3}");
}

TEST_CASE("Arguments can non-sequential.", "[invocation][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${2} ${1}";
        }
        return {};
    };

    Invocation invocation;
    invocation.setCmdLine({ "alias", "arg1", "arg2" });
    invocation.setAliasResolver(aliasResolver);
    invocation.parse();

    std::string cmd = invocation.getCmdName();
    std::vector<std::string> args = invocation.getCmdArgs();

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "arg2");
    REQUIRE(args[1] == "arg1");
}

TEST_CASE("Absent arguments are expanded to empty strings.",
          "[invocation][aliases]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${10} ${11}";
        }
        return {};
    };

    Invocation invocation;
    invocation.setCmdLine({ "alias" });
    invocation.setAliasResolver(aliasResolver);
    invocation.parse();

    std::string cmd = invocation.getCmdName();
    std::vector<std::string> args = invocation.getCmdArgs();

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "");
    REQUIRE(args[1] == "");
}

TEST_CASE("Completion stops after last argument is inserted.",
          "[invocation][aliases][completion]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias") {
            return "cmd ${2} ${3} ${1}";
        }
        return {};
    };

    Invocation invocation;
    invocation.setCmdLine({ "alias", "arg1", "arg2", "arg3" });
    invocation.setAliasResolver(aliasResolver);
    invocation.parse(true);

    std::string cmd = invocation.getCmdName();
    std::vector<std::string> args = invocation.getCmdArgs();

    REQUIRE(cmd == "cmd");
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "arg2");
    REQUIRE(args[1] == "arg3");
}
