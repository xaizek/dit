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

#include <sstream>

#include "Command.hpp"
#include "Commands.hpp"
#include "Config.hpp"
#include "Dit.hpp"
#include "Project.hpp"

#include "Tests.hpp"

TEST_CASE("Config value completion on trailing =", "[cmds][config][completion]")
{
    Tests::disableDecorations();

    Command *const cmd = Commands::get("config");

    std::unique_ptr<Project> prj = Tests::makeProject();

    Config &cfg = prj->getConfig(false);
    cfg.set("ui.ls", "ls-value");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    static char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app", "projects" });

    // Initialize internal state.
    REQUIRE(!cmd->complete(dit, { }));

    boost::optional<int> exitCode = cmd->complete(*prj, { "ui.ls=" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut = "'ls-value'\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Config completes options", "[cmds][config][completion]")
{
    Tests::disableDecorations();

    Command *const cmd = Commands::get("config");

    std::unique_ptr<Project> prj = Tests::makeProject();

    Config &cfg = prj->getConfig(false);
    cfg.set("ui.ls", "ls-value");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    static char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app", "projects" });

    // Initialize internal state.
    REQUIRE(!cmd->complete(dit, { }));

    boost::optional<int> exitCode = cmd->complete(*prj, { "-" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut =
        "--help\n"
        "--global\n"
        "-h\n"
        "-g\n"
        "ui.ls:\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Config completes global config", "[cmds][config][completion]")
{
    Tests::disableDecorations();

    Command *const cmd = Commands::get("config");

    std::unique_ptr<Project> prj = Tests::makeProject();

    // This shouldn't appear in completion results.
    Config &cfg = prj->getConfig(false);
    cfg.set("ui.ls", "ls-value");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    static char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app", "projects" });

    boost::optional<int> exitCode;

    exitCode = cmd->complete(dit, { "--global", "" });
    REQUIRE(!exitCode);

    exitCode = cmd->complete(*prj, { "--global", "" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut =
        "--help\n"
        "--global\n"
        "-h\n"
        "-g\n"
        "core.defprj:\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}
