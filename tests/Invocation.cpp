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

#include <string>
#include <vector>

#include "Catch/catch.hpp"

#include "Invocation.hpp"

TEST_CASE("Project name is extracted correctly.", "[invocation][project]")
{
    auto aliasResolver = [](const std::string &) { return std::string(); };

    Invocation invocation;
    invocation.setCmdLine({ ".proj" });
    invocation.setAliasResolver(aliasResolver);
    invocation.parse();

    REQUIRE(invocation.getPrjName() == "proj");
}

TEST_CASE("Default project name is used.", "[invocation][project][defaults]")
{
    auto aliasResolver = [](const std::string &) { return std::string(); };

    Invocation invocation;
    invocation.setDefPrjName("def");
    invocation.setAliasResolver(aliasResolver);
    invocation.parse();

    REQUIRE(invocation.getPrjName() == "def");
}

TEST_CASE("Default command-line is used.", "[invocation][defaults]")
{
    auto aliasResolver = [](const std::string &) { return std::string(); };

    Invocation invocation;
    invocation.setDefCmdLine("defcmd 'with' \"args\" goes\\ here");
    invocation.setAliasResolver(aliasResolver);
    invocation.parse();

    std::string cmd = invocation.getCmdName();
    std::vector<std::string> args = invocation.getCmdArgs();

    REQUIRE(cmd == "defcmd");
    REQUIRE(args.size() == 3);
    REQUIRE(args[0] == "with");
    REQUIRE(args[1] == "args");
    REQUIRE(args[2] == "goes here");
}

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

TEST_CASE("", "[invocation]")
{
    Invocation invocation;
    invocation.setCmdLine({ "alias" });

    REQUIRE_THROWS_AS(invocation.parse(), std::bad_function_call);
}

TEST_CASE("Aliases are decomposed.", "[invocation][alias][composition]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias1") {
            return "prefix1 ${1}";
        }
        if (name == "alias2") {
            return "prefix2";
        }
        return {};
    };

    Invocation invocation;
    invocation.setCmdLine({ "alias2.alias1", "arg1", "arg2", "arg3" });
    invocation.setAliasResolver(aliasResolver);
    invocation.parse(true);

    std::string cmd = invocation.getCmdName();
    std::vector<std::string> args = invocation.getCmdArgs();

    REQUIRE(cmd == "prefix2");
    REQUIRE(args.size() == 4);
    REQUIRE(args[0] == "prefix1");
    REQUIRE(args[1] == "arg1");
    REQUIRE(args[2] == "arg2");
    REQUIRE(args[3] == "arg3");
}

TEST_CASE("Alias composition configuration is collected.",
          "[invocation][alias][composition]")
{
    auto aliasResolver = [](const std::string &name) -> std::string {
        if (name == "alias1") {
            return "a=1 ls ${1}";
        }
        if (name == "alias2") {
            return "a+=2";
        }
        return {};
    };

    Invocation invocation;
    invocation.setCmdLine({ "alias2.alias1.cmd", "arg1", "arg2", "arg3" });
    invocation.setAliasResolver(aliasResolver);
    invocation.parse(true);

    std::string cmd = invocation.getCmdName();
    std::vector<std::string> args = invocation.getCmdArgs();
    using conf = std::pair<std::string, std::string>;
    std::vector<conf> confs = invocation.getConfs();

    REQUIRE(cmd == "ls");
    REQUIRE(args.size() == 4);
    REQUIRE(args[0] == "cmd");
    REQUIRE(args[1] == "arg1");
    REQUIRE(args[2] == "arg2");
    REQUIRE(args[3] == "arg3");
    REQUIRE(confs.size() == 2);
    REQUIRE(confs[0] == conf("a", "1"));
    REQUIRE(confs[1] == conf("a+", "2"));
}
