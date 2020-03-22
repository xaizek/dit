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

#include <cstdlib>

#include <sstream>
#include <string>

#include "Change.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"

#include "Tests.hpp"

TEST_CASE("Ls lists items", "[cmds][ls]")
{
    Tests::disableDecorations();

    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    prj->getConfig().set("ui.ls.fmt", "_id");
    prj->getConfig().set("ui.ls.sort", "_id");
    prj->getConfig().set("ui.ls.color", "inv !heading");

    Command *const cmd = Commands::get("ls");
    boost::optional<int> exitCode = cmd->run(*prj, { "bug_number/2" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    REQUIRE(out.str() == "ID\nid\n");
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Ls completes fields", "[cmds][ls][completion]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    Command *const cmd = Commands::get("ls");
    boost::optional<int> exitCode = cmd->complete(*prj, { });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    REQUIRE(out.str() == "bug_number\ntitle\n");
    REQUIRE(err.str() == std::string());
}
