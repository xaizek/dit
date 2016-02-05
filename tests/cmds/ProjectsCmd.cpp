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

#include "Command.hpp"
#include "Commands.hpp"
#include "Dit.hpp"

#include "Tests.hpp"

TEST_CASE("Projects command", "[cmds][projects][invocation]")
{
    Command *const cmd = Commands::get("projects");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    char xdg_env[] = "XDG_CONFIG_HOME=tests/no-data";
    char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app" });

    SECTION("Expects no arguments")
    {
        boost::optional<int> exitCode = cmd->run(dit, { "a" });

        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_FAILURE);

        REQUIRE(out.str() == std::string());
        REQUIRE(err.str() != std::string());
    }

    SECTION("Projects does nothing on absent configuration directory")
    {
        boost::optional<int> exitCode = cmd->run(dit, { });

        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        REQUIRE(out.str() == std::string());
        REQUIRE(err.str() == std::string());
    }
}
