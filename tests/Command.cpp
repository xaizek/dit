// Copyright (C) 2016 xaizek <xaizek@posteo.net>
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

#include <string>
#include <utility>

#include "Command.hpp"
#include "Dit.hpp"
#include "Project.hpp"

#include "Tests.hpp"

class Cmd : public Command
{
public:
    Cmd(std::string name, std::string descr, std::string help)
        : Command(std::move(name), std::move(descr), std::move(help))
    {
    }
};

TEST_CASE("Command accessors work", "[command]")
{
    Cmd cmd("test-cmd", "descr", "help");
    REQUIRE(cmd.getName() == "test-cmd");
    REQUIRE(cmd.getDescr() == "descr");
    REQUIRE(cmd.getHelp() == "help");
}

TEST_CASE("Default functions return empty optional", "[command]")
{
    static char xdg_env[] = "XDG_CONFIG_HOME=tests/data";
    static char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Dit dit({ "app", "projects" });
    std::unique_ptr<Project> prj = Tests::makeProject();

    Cmd cmd("test-cmd", "descr", "help");
    REQUIRE(!cmd.run(dit, {}));
    REQUIRE(!cmd.run(*prj, {}));
    REQUIRE(!cmd.complete(dit, {}));
    REQUIRE(!cmd.complete(*prj, {}));
}
