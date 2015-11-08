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

#include "Catch/catch.hpp"

#include <cstdlib>

#include <sstream>

#include "Config.hpp"
#include "Project.hpp"
#include "Scribe.hpp"

#include "Tests.hpp"

TEST_CASE("Running commands", "[app]")
{
    Tests::disableDecorations();

    char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    SECTION("Projects")
    {
        Scribe scribe({ "app", "projects" });

        std::ostringstream out;
        std::ostringstream err;
        Tests::setStreams(out, err);

        boost::optional<int> exitCode = scribe.run();
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut =
            " first\n"
            " second\n"
            "*tests\n"
            " third\n";
        REQUIRE(out.str() == expectedOut);
        REQUIRE(err.str() == std::string());
    }

    SECTION("Ls")
    {
        Scribe scribe({ "app", "ls" });

        std::ostringstream out;
        std::ostringstream err;
        Tests::setStreams(out, err);

        boost::optional<int> exitCode = scribe.run();
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut = "ID  TITLE\n";
        REQUIRE(out.str() == expectedOut);
        REQUIRE(err.str() == std::string());
    }
}

TEST_CASE("Completion of projects", "[app][completion]")
{
    Tests::disableDecorations();

    char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Scribe scribe({ "app" });

    std::ostringstream out;
    std::ostringstream err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = scribe.complete({ ".::cursor::" },
                                                    out, err);
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut =
        ".first\n"
        ".second\n"
        ".tests\n"
        ".third\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Completion of commands", "[app][completion]")
{
    Tests::disableDecorations();

    char xdg_env[] = "XDG_CONFIG_HOME=";
    char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Scribe scribe({ "app" });

    std::ostringstream out;
    std::ostringstream err;
    Tests::setStreams(out, err);

    SECTION("No prefix")
    {
        boost::optional<int> exitCode = scribe.complete({ "::cursor::" },
                                                        out, err);
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut =
            "add\n"
            "complete\n"
            "config\n";
        REQUIRE(out.str().substr(0, expectedOut.length()) == expectedOut);
        REQUIRE(err.str() == std::string());
    }

    SECTION("Composition")
    {
        boost::optional<int> exitCode = scribe.complete({ "add.::cursor::" },
                                                        out, err);
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut =
            "add.add\n"
            "add.complete\n"
            "add.config\n";
        REQUIRE(out.str().substr(0, expectedOut.length()) == expectedOut);
        REQUIRE(err.str() == std::string());
    }
}

TEST_CASE("Completion of sub-commands", "[app][completion]")
{
    Tests::disableDecorations();

    char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Scribe scribe({ "app" });

    std::ostringstream out;
    std::ostringstream err;
    Tests::setStreams(out, err);

    SECTION("Works")
    {
        boost::optional<int> exitCode = scribe.complete({ "config",
                                                          "ui.::cursor::" },
                                                        out, err);
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut =
            "ui.ls:\n"
            "ui.show:\n";
        REQUIRE(out.str() == expectedOut);
        REQUIRE(err.str() == std::string());
    }

    SECTION("Works for value completion")
    {
        boost::optional<int> exitCode = scribe.complete({ "config",
                                                          "ui.ls=::cursor::" },
                                                        out, err);
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut = "'ls-value'\n";
        REQUIRE(out.str() == expectedOut);
        REQUIRE(err.str() == std::string());
    }

    SECTION("Uses specified project")
    {
        boost::optional<int> exitCode = scribe.complete({ ".first", "config",
                                                          "ui.::cursor::" },
                                                        out, err);
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        REQUIRE(out.str() == std::string());
        REQUIRE(err.str() == std::string());
    }
}
