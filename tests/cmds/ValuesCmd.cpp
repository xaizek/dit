// Copyright (C) 2018 xaizek <xaizek@posteo.net>
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

#include "Change.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Config.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"

#include "Tests.hpp"

TEST_CASE("Values fails on wrong invocation", "[cmds][values][args]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Command *const cmd = Commands::get("values");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode;

    SECTION("Fails on zero arguments")
    {
        exitCode = cmd->run(*prj, { });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_FAILURE);
        REQUIRE(out.str() == std::string());
        REQUIRE(err.str() != std::string());
    }

    SECTION("Too many arguments")
    {
        exitCode = cmd->run(*prj, { "a", "b" });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_FAILURE);
        REQUIRE(out.str() == std::string());
        REQUIRE(err.str() != std::string());
    }
}

TEST_CASE("Values of a key are listed", "[cmds][values][args]")
{
    Command *const cmd = Commands::get("values");
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item1 = Tests::makeItem("id1");
    item1.setValue("title", "something");
    Tests::storeItem(storage, std::move(item1));

    Item item2 = Tests::makeItem("id2");
    item2.setValue("title", "something-else");
    Tests::storeItem(storage, std::move(item2));

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->run(*prj, { "title" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    REQUIRE(out.str() == "something\nsomething-else\n");
    REQUIRE(err.str() == std::string());
}
