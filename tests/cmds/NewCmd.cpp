// Copyright (C) 2016 xaizek <xaizek@posteo.net>
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

#include "Catch/catch.hpp"

#include <boost/filesystem/operations.hpp>

#include <cstdlib>

#include <sstream>

#include "Command.hpp"
#include "Commands.hpp"
#include "Dit.hpp"

#include "Tests.hpp"

namespace fs = boost::filesystem;

TEST_CASE("New invocation", "[cmds][new][invocation]")
{
    Command *const cmd = Commands::get("new");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    static char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app" });

    boost::optional<int> exitCode;

    SECTION("Fails on zero arguments")
    {
        exitCode = cmd->run(dit, { });
    }

    SECTION("Fails on too many arguments")
    {
        exitCode = cmd->run(dit, { "a", "b" });
    }

    SECTION("Project name can't contain slash")
    {
        exitCode = cmd->run(dit, { "a/b/c" });
    }

    SECTION("Fails if project already exists")
    {
        exitCode = cmd->run(dit, { "first" });
    }

    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_FAILURE);

    REQUIRE(out.str() == std::string());
    REQUIRE(err.str() != std::string());
}

TEST_CASE("New creates new project", "[cmds][new]")
{
    Command *const cmd = Commands::get("new");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    static char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app" });

    boost::optional<int> exitCode = cmd->run(dit, { "test" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    fs::remove_all("tests/data/dit/projects/test");
}
