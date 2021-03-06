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

TEST_CASE("Set fails on wrong invocation", "[cmds][set][invocation]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Command *const cmd = Commands::get("set");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode;

    SECTION("No arguments")
    {
        exitCode = cmd->run(*prj, {});
    }

    SECTION("One argument")
    {
        exitCode = cmd->run(*prj, {});
    }

    SECTION("Malformed key name")
    {
        Storage &storage = prj->getStorage();

        Item item = Tests::makeItem("id");
        item.setValue("title", "title");
        item.setValue("bug_number", "22");

        Tests::storeItem(storage, std::move(item));

        exitCode = cmd->run(*prj, { "id", "title=1", "!@$=a" });
        REQUIRE(storage.get("id").getValue("title") == "title");
        REQUIRE(err.str() != std::string());
        err.clear();
    }

    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_FAILURE);

    REQUIRE(out.str() == std::string());
    REQUIRE(err.str() != std::string());
}

TEST_CASE("Set allows external editing", "[cmds][set][integration]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    static char editor_env[] = "EDITOR=echo new-value >>";
    putenv(editor_env);

    Command *const cmd = Commands::get("set");
    boost::optional<int> exitCode = cmd->run(*prj, { "id", "title+=-",
                                                     "bug_number=-" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    REQUIRE(out.str() == std::string());
    REQUIRE(err.str() == std::string());

    REQUIRE(storage.get("id").getValue("title") == "title\nnew-value");
    REQUIRE(storage.get("id").getValue("bug_number") == "22new-value");
}

TEST_CASE("Nothing is set on failed external editing",
          "[cmds][set][integration]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "oldtitle");
    item.setValue("author", "someone");

    Tests::storeItem(storage, std::move(item));

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    static char editor_env[] = "EDITOR=wrong-command >>";
    putenv(editor_env);

    Command *const cmd = Commands::get("set");
    boost::optional<int> exitCode = cmd->run(*prj, { "id",
                                                     "title:",  "title",
                                                     "status:", "-",
                                                     "author:", "me" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_FAILURE);

    REQUIRE(out.str() == std::string());
    REQUIRE(err.str() != std::string());

    REQUIRE(storage.get("id").getValue("title") == "oldtitle");
    REQUIRE(storage.get("id").getValue("status") == std::string());
    REQUIRE(storage.get("id").getValue("author") == "someone");
}

TEST_CASE("Item is changed successfully", "[cmds][set]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("set");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode;

    SECTION("Set")
    {
        exitCode = cmd->run(*prj, { "id", "bug_number=33" });
        REQUIRE(storage.get("id").getValue("bug_number") == "33");
    }

    SECTION("Appended")
    {
        exitCode = cmd->run(*prj, { "id", "title+=more" });
        REQUIRE(storage.get("id").getValue("title") == "title\nmore");
    }

    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    REQUIRE(out.str() == std::string());
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Completion of id for set", "[cmds][set][completion]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("set");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->complete(*prj, { "i" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut = "id\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Completion of first key name on set", "[cmds][set][completion]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("set");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->complete(*prj, { "id", "ti" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut =
        "bug_number:\n"
        "title:\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Completion of next key name on set", "[cmds][set][completion]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("set");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->complete(*prj,
                                                  { "id", "title:", "t", "b" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut = "bug_number:\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Completion of a value on set", "[cmds][set][completion]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("set");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->complete(*prj, { "id", "title=" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut = "title\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}
