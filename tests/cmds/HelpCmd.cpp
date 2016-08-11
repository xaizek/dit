// Copyright (C) 2016 xaizek <xaizek@openmailbox.org>
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

#include "Catch/catch.hpp"

#include <cstdlib>

#include <sstream>
#include <string>

#include "Command.hpp"
#include "Commands.hpp"
#include "Dit.hpp"

#include "Tests.hpp"

TEST_CASE("Help checks arguments", "[cmds][help][invocation]")
{
    Tests::disableDecorations();

    Command *const cmd = Commands::get("help");

    Dit dit({ "app" });

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode;

    SECTION("Too many arguments")
    {
        exitCode = cmd->run(dit, { "asdf", "bla" });
    }

    SECTION("Wrong command name")
    {
        exitCode = cmd->run(dit, { "asdf" });
    }

    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_FAILURE);
    REQUIRE(out.str() == std::string());
    REQUIRE(err.str() != std::string());
}

TEST_CASE("Help lists all commands", "[cmds][help]")
{
    Tests::disableDecorations();

    Command *const cmd = Commands::get("help");

    Dit dit({ "app" });

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->run(dit, { });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut =
        "add -- add new item\n"
        "check -- verify project state\n"
        "complete -- perform command-line completion\n";
    REQUIRE(out.str().substr(0, expectedOut.length()) == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Help displays help on a command", "[cmds][help]")
{
    Tests::disableDecorations();

    Command *const cmd = Commands::get("help");

    Dit dit({ "app" });

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->run(dit, { "show" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut =
        "show -- display item entries\n"
        "\n"
        "Usage: show id [key...]\n";
    REQUIRE(out.str().substr(0, expectedOut.length()) == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Help is completed", "[cmds][help][completion]")
{
    Tests::disableDecorations();

    Command *const cmd = Commands::get("help");

    Dit dit({ "app" });

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    SECTION("Too many arguments")
    {
        boost::optional<int> exitCode = cmd->complete(dit, { "asdf", "bla" });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_FAILURE);
        REQUIRE(out.str() == std::string());
        REQUIRE(err.str() == std::string());
    }

    SECTION("Normal amout of arguments")
    {
        boost::optional<int> exitCode = cmd->complete(dit, { "asdf" });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut =
            "add\n"
            "check\n"
            "complete\n";
        REQUIRE(out.str().substr(0, expectedOut.length()) == expectedOut);
        REQUIRE(err.str() == std::string());
    }
}
