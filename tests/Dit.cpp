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

#include "Catch/catch.hpp"

#include <cstdlib>

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Config.hpp"
#include "Dit.hpp"
#include "Project.hpp"

#include "Tests.hpp"

TEST_CASE("Dit throws exception on empty args", "[app][invocation]")
{
    REQUIRE_THROWS_AS(Dit({}), std::runtime_error);
}

TEST_CASE("Dit throws exception on absent $HOME", "[app][invocation]")
{
    std::string home = std::getenv("HOME");

    unsetenv("HOME");
    REQUIRE_THROWS_AS(Dit({ "dit" }), std::runtime_error);

    static std::string homeSet = "HOME=" + home;
    putenv(&homeSet[0]);
}

TEST_CASE("Help is displayed", "[app][invocation]")
{
    StreamCapture capture(std::cout);

    Dit dit({ "app", "--help" });

    boost::optional<int> exitCode = dit.run();
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);
    REQUIRE(!capture.get().empty());
}

TEST_CASE("Version is displayed", "[app][invocation]")
{
    StreamCapture capture(std::cout);

    Dit dit({ "app", "--version" });

    boost::optional<int> exitCode = dit.run();
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);
    REQUIRE(!capture.get().empty());
}

TEST_CASE("Dit errors on invalid project name", "[app][invocation]")
{
    StreamCapture coutCapture(std::cout), cerrCapture(std::cerr);

    static char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app", ".wrong-proj-name", "ls" });

    boost::optional<int> exitCode = dit.run();
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_FAILURE);

    REQUIRE(coutCapture.get() == std::string());
    REQUIRE(cerrCapture.get() != std::string());
}

TEST_CASE("Dit errors on empty project name", "[app][invocation][completion]")
{
    StreamCapture coutCapture(std::cout), cerrCapture(std::cerr);

    static char xdg_env[] = "XDG_CONFIG_HOME=tests/no-such-dir";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app", ".", "ls" });

    boost::optional<int> exitCode;

    SECTION("Invocation")
    {
        exitCode = dit.run();
    }

    SECTION("Completion")
    {
        std::ostringstream out;
        std::ostringstream err;
        exitCode = dit.complete({ ".", "ls" }, out, err);
    }

    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_FAILURE);

    REQUIRE(coutCapture.get() == std::string());
    REQUIRE(cerrCapture.get() != std::string());
}

TEST_CASE("Empty project and command are allowed",
          "[app][invocation][completion]")
{
    StreamCapture coutCapture(std::cout), cerrCapture(std::cerr);

    // Use wrong root directory to get empty global configuration.
    static char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app" });
    dit.getConfig().set("defcmd", "rename");

    std::ostringstream out;
    std::ostringstream err;
    boost::optional<int> exitCode = dit.complete({ ".", "", "f::cursor::" },
                                                 out, err);
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut =
        "first\n"
        "second\n"
        "tests\n"
        "third\n";
    REQUIRE(coutCapture.get() == expectedOut);
    REQUIRE(cerrCapture.get() == std::string());
}

TEST_CASE("Running commands", "[app]")
{
    Tests::disableDecorations();

    static char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    std::ostringstream out;
    std::ostringstream err;
    Tests::setStreams(out, err);

    SECTION("Wrong command name")
    {
        StreamCapture coutCapture(std::cout), cerrCapture(std::cerr);

        Dit dit({ "app", "wrong-name" });

        boost::optional<int> exitCode = dit.run();
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_FAILURE);

        REQUIRE(coutCapture.get() == std::string());
        REQUIRE(cerrCapture.get() != std::string());
    }

    SECTION("projects")
    {
        Dit dit({ "app", "projects" });

        boost::optional<int> exitCode = dit.run();
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut =
            " first\n"
            " second\n"
            "*tests\n"
            " third -- third test project\n";
        REQUIRE(out.str() == expectedOut);
        REQUIRE(err.str() == std::string());
    }

    SECTION("ls")
    {
        Dit dit({ "app", "ls" });

        boost::optional<int> exitCode = dit.run();
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut = "ID  TITLE\n";
        REQUIRE(out.str() == expectedOut);
        REQUIRE(err.str() == std::string());
    }

    SECTION("complete")
    {
        Dit dit({ "app", "complete", ".first", "config", "ui.::cursor::" });

        boost::optional<int> exitCode = dit.run();
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut =
            "--help\n"
            "--global\n"
            "-h\n"
            "-g\n";

        REQUIRE(out.str() == expectedOut);
        REQUIRE(err.str() == std::string());
    }

    SECTION("Configuration override append to default")
    {
        Dit dit({ "app", "ui.ls.fmt+=,more", "ls" });

        boost::optional<int> exitCode = dit.run();
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut = "ID  TITLE  MORE\n";
        REQUIRE(out.str() == expectedOut);
        REQUIRE(err.str() == std::string());
    }

    SECTION("Configuration override set and append")
    {
        Dit dit({ "app", "ui.ls.fmt=_id", "ui.ls.fmt+=,more", "ls" });

        boost::optional<int> exitCode = dit.run();
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut = "ID  MORE\n";
        REQUIRE(out.str() == expectedOut);
        REQUIRE(err.str() == std::string());
    }
}

TEST_CASE("Completion of projects", "[app][completion]")
{
    Tests::disableDecorations();

    static char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app" });

    std::ostringstream out;
    std::ostringstream err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = dit.complete({ ".::cursor::" }, out, err);
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

    static char xdg_env[] = "XDG_CONFIG_HOME=";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app" });

    std::ostringstream out;
    std::ostringstream err;
    Tests::setStreams(out, err);

    SECTION("No prefix")
    {
        dit.getConfig().set("alias.a__", "abc");

        boost::optional<int> exitCode = dit.complete({ "::cursor::" },
                                                     out, err);
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut =
            "--help\n"
            "--version\n"
            "-h\n"
            "-v\n"
            "a__\n"
            "add\n"
            "check\n"
            "complete\n"
            "config\n";
        REQUIRE(out.str().substr(0, expectedOut.length()) == expectedOut);
        REQUIRE(err.str() == std::string());
    }

    SECTION("Composition")
    {
        boost::optional<int> exitCode = dit.complete({ "add.::cursor::" },
                                                     out, err);
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut =
            "--help\n"
            "--version\n"
            "-h\n"
            "-v\n"
            "add.check\n"
            "add.complete\n"
            "add.config\n";
        REQUIRE(out.str().substr(0, expectedOut.length()) == expectedOut);
        REQUIRE(err.str() == std::string());
    }
}

TEST_CASE("Completion of sub-commands", "[app][completion]")
{
    Tests::disableDecorations();

    static char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app" });

    std::ostringstream out;
    std::ostringstream err;
    Tests::setStreams(out, err);

    SECTION("Wrong sub-command name")
    {
        boost::optional<int> exitCode = dit.complete({ "asdf",
                                                       "ui.::cursor::" },
                                                     out, err);
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_FAILURE);

        REQUIRE(out.str() == std::string());
        REQUIRE(err.str() == std::string());
    }

    SECTION("Works")
    {
        boost::optional<int> exitCode = dit.complete({ "config",
                                                       "ui.::cursor::" },
                                                     out, err);
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut =
            "--help\n"
            "--global\n"
            "-h\n"
            "-g\n"
            "ui.ls:\n"
            "ui.show:\n";
        REQUIRE(out.str() == expectedOut);
        REQUIRE(err.str() == std::string());
    }

    SECTION("Works for value completion")
    {
        boost::optional<int> exitCode = dit.complete({ "config",
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
        boost::optional<int> exitCode = dit.complete({ ".first", "config",
                                                       "ui.::cursor::" },
                                                     out, err);
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        const std::string expectedOut =
            "--help\n"
            "--global\n"
            "-h\n"
            "-g\n";

        REQUIRE(out.str() == expectedOut);
        REQUIRE(err.str() == std::string());
    }
}
