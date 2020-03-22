// Copyright (C) 2015 xaizek <xaizek@posteo.net>
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

#include "Change.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Config.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"

#include "Tests.hpp"

TEST_CASE("Too few args cause error.", "[cmds][show][args]")
{
    Command *const cmd = Commands::get("show");
    std::unique_ptr<Project> prj = Tests::makeProject();

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->run(*prj, { });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_FAILURE);

    REQUIRE(out.str() == std::string());
    REQUIRE(err.str() != std::string());
}

TEST_CASE("Values are filtered.", "[cmds][show]")
{
    Tests::disableDecorations();

    Command *const cmd = Commands::get("show");
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();
    Config &cfg = prj->getConfig();

    cfg.set("ui.show.order", "wrongfieldname");

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->run(*prj, { "id", "title" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut =
        "title: title\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Values are displayed in alphabetical order by default.",
          "[cmds][show][order]")
{
    Tests::disableDecorations();

    Command *const cmd = Commands::get("show");
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();
    Config &cfg = prj->getConfig();

    cfg.set("ui.show.order", "wrongfieldname");

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->run(*prj, { "id" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut =
        "bug_number: 22\n"
        "title: title\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Values are displayed in specified order.", "[cmds][show][order]")
{
    Tests::disableDecorations();

    Command *const cmd = Commands::get("show");
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();
    Config &cfg = prj->getConfig();

    cfg.set("ui.show.order", "title,comment");

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("comment", "something");
    item.setValue("bug_number", "22");
    item.setValue("additional", "information");

    Tests::storeItem(storage, std::move(item));

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->run(*prj, { "id" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut =
        "title: title\n"
        "comment: something\n"
        "additional: information\n"
        "bug_number: 22\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Completion of id for show", "[cmds][show][completion]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("show");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->complete(*prj, { "i" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut = "id\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Completion of field name for show", "[cmds][show][completion]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("show");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    std::string expectedOut;

    SECTION("Wrong id produces error")
    {
        boost::optional<int> exitCode = cmd->complete(*prj, { "idd", "ti" });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_FAILURE);
    }

    SECTION("First filter value")
    {
        boost::optional<int> exitCode = cmd->complete(*prj, { "id", "ti" });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        expectedOut =
            "bug_number\n"
            "title\n";
    }

    SECTION("Second filter value")
    {
        boost::optional<int> exitCode = cmd->complete(*prj,
                                                      { "id", "title", "b" });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        expectedOut = "bug_number\n";
    }

    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}
