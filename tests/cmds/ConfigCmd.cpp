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

TEST_CASE("Config handles options", "[cmds][config][invocation]")
{
    Command *const cmd = Commands::get("config");

    Dit dit({ "app" });

    SECTION("--help")
    {
        std::ostringstream out, err;
        Tests::setStreams(out, err);

        boost::optional<int> exitCode = cmd->run(dit, { "--help" });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);
        REQUIRE(out.str() != std::string());
        REQUIRE(err.str() == std::string());
    }

    SECTION("Global run does nothing without --global")
    {
        boost::optional<int> exitCode = cmd->run(dit, { "something" });
        REQUIRE(!exitCode);
    }
}

TEST_CASE("Invalid key names cause errors", "[cmds][config]")
{
    Command *const cmd = Commands::get("config");

    std::unique_ptr<Project> prj = Tests::makeProject();

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode;

    SECTION("Empty key name")
    {
        exitCode = cmd->run(*prj, { "=af" });
    }

    SECTION("Builtin key name")
    {
        exitCode = cmd->run(*prj, { "!key=af" });
    }

    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);
    REQUIRE(out.str() != std::string());
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Key values are printed", "[cmds][config]")
{
    Command *const cmd = Commands::get("config");

    std::unique_ptr<Project> prj = Tests::makeProject();

    prj->getConfig(false).set("asdf", "jkl;");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->run(*prj, { "asdf" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);
    REQUIRE(out.str() == "asdf = jkl;\n");
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Config allows external editing", "[cmds][config][integration]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    static char editor_env[] = "EDITOR=echo new-value >>";
    putenv(editor_env);

    Command *const cmd = Commands::get("config");
    boost::optional<int> exitCode = cmd->run(*prj, { "value=-" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    REQUIRE(out.str() == std::string());
    REQUIRE(err.str() == std::string());

    REQUIRE(prj->getConfig(true).get("value") == "new-value");
}

TEST_CASE("Global configuration is processed", "[cmds][config]")
{
    Command *const cmd = Commands::get("config");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    static char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app", "--version" });

    SECTION("Displayed")
    {
        const std::string expectedOut =
            "core.defcmd = rename\n"
            "core.defprj = tests\n";

        boost::optional<int> exitCode = cmd->run(dit, { "--global" });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);
        REQUIRE(out.str() == expectedOut);
        REQUIRE(err.str() == std::string());
    }

    SECTION("Added")
    {
        boost::optional<int> exitCode = cmd->run(dit, { "--global", "a=b" });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);
        REQUIRE(out.str() == std::string());
        REQUIRE(err.str() == std::string());

        REQUIRE(dit.getConfig().get("a") == "b");
    }

    SECTION("Updated")
    {
        boost::optional<int> exitCode = cmd->run(dit, { "--global",
                                                        "core.defprj=_" });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);
        REQUIRE(out.str() == std::string());
        REQUIRE(err.str() == std::string());

        REQUIRE(dit.getConfig().get("core.defprj") == "_");
    }
}

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
        "core.defcmd:\n"
        "core.defprj:\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Matches omit present elements", "[cmds][config][completion]")
{
    Tests::disableDecorations();

    std::unique_ptr<Project> prj = Tests::makeProject();

    Command *const cmd = Commands::get("config");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    static char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app" });
    Config &cfg = dit.getConfig();
    cfg.set("_a", "b");
    cfg.set("_c", "d");
    cfg.set("_e", "f");

    // Initialize internal state.
    REQUIRE(!cmd->complete(dit, { }));

    boost::optional<int> exitCode = cmd->complete(*prj,
                                                  { "--global",
                                                    "_a=nr", "_c:", "x", "_" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut =
        "--help\n"
        "--global\n"
        "-h\n"
        "-g\n"
        "_e:\n"
        "core.defcmd:\n"
        "core.defprj:\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}
