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

#include <sys/stat.h>

#include <cstdlib>

#include <sstream>

#include "Command.hpp"
#include "Commands.hpp"
#include "Dit.hpp"

#include "Tests.hpp"

TEST_CASE("Rename command fails", "[cmds][rename][invocation]")
{
    Command *const cmd = Commands::get("rename");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    static char no_data[] = "XDG_CONFIG_HOME=tests/no-data";
    static char data[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(home_env);

    putenv(no_data);
    Dit badDit({ "app" });

    putenv(data);
    Dit goodDit({ "app" });

    boost::optional<int> exitCode;

    SECTION("Fails on too few args")
    {
        exitCode = cmd->run(badDit, { "a" });
    }

    SECTION("Fails on too many args")
    {
        exitCode = cmd->run(badDit, { "a", "b", "c" });
    }

    SECTION("Fails for non-existent source")
    {
        exitCode = cmd->run(badDit, { "a", "b" });
    }

    SECTION("Fails for existing destination")
    {
        exitCode = cmd->run(goodDit, { "first", "second" });
    }

    SECTION("Fails for source with slash")
    {
        exitCode = cmd->run(goodDit, { "a/b", "b" });
    }

    SECTION("Fails for destination with slash")
    {
        exitCode = cmd->run(goodDit, { "first", "seco/nd" });
    }

    SECTION("Fails for destination with slash")
    {
        REQUIRE(chmod("tests/data/dit/projects", 0555) == 0);
        try {
            exitCode = cmd->run(goodDit, { "first", "x" });
        } catch (...) {
            REQUIRE(chmod("tests/data/dit/projects", 0755) == 0);
            throw;
        }
        REQUIRE(chmod("tests/data/dit/projects", 0755) == 0);
    }

    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_FAILURE);

    REQUIRE(out.str() == std::string());
    REQUIRE(err.str() != std::string());
}

TEST_CASE("Rename command succeeds", "[cmds][rename][invocation]")
{
    Command *const cmd = Commands::get("rename");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    static char data[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(data);
    putenv(home_env);

    Dit dit({ "app" });

    boost::optional<int> exitCode;

    exitCode = cmd->run(dit, { "first", "x" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    exitCode = cmd->run(dit, { "x", "first" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    REQUIRE(out.str() == std::string());
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Rename command successful completion", "[cmds][rename][completion]")
{
    Command *const cmd = Commands::get("rename");

    static char data[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(data);
    putenv(home_env);

    Dit dit({ "app" });

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode;

    SECTION("No args")
    {
        exitCode = cmd->complete(dit, { });
    }

    SECTION("One arg")
    {
        exitCode = cmd->complete(dit, { "a" });
    }

    SECTION("Two args")
    {
        exitCode = cmd->complete(dit, { "a", "b" });
    }

    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut =
        "first\n"
        "second\n"
        "tests\n"
        "third\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Rename failed successful completion", "[cmds][rename][completion]")
{
    Command *const cmd = Commands::get("rename");

    static char data[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(data);
    putenv(home_env);

    Dit dit({ "app" });

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->complete(dit, { "a", "b", "c" });

    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_FAILURE);

    REQUIRE(out.str() == std::string());
    REQUIRE(err.str() == std::string());
}
