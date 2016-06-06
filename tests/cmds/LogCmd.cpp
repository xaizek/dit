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

#include <boost/algorithm/string/predicate.hpp>

#include <cstdlib>

#include <sstream>
#include <stdexcept>

#include "utils/strings.hpp"
#include "Change.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"

#include "Tests.hpp"

TEST_CASE("Log fails on wrong invocation", "[cmds][log][invocation]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Command *const cmd = Commands::get("log");

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

    SECTION("Wrong id")
    {
        REQUIRE_THROWS_AS(cmd->run(*prj, { "asdf" }), std::runtime_error);
    }
}

TEST_CASE("Log filters fields", "[cmds][log]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Command *const cmd = Commands::get("log");
    Storage &storage = prj->getStorage();

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    Item item = Tests::makeItem("id");
    item.setValue("title", "something");
    item.setValue("bug_number", "22");
    Tests::storeItem(storage, std::move(item));

    boost::optional<int> exitCode = cmd->run(*prj, { "id", "title" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::vector<std::string> lines = split(out.str(), '\n');
    REQUIRE(boost::starts_with(lines[0], "title created:"));
    REQUIRE(lines[1] == std::string());
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Log displays operations correctly", "[cmds][log]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Command *const cmd = Commands::get("log");
    Storage &storage = prj->getStorage();

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    std::time_t t = std::time(nullptr);
    MockTimeSource timeMock([&t](){ return t++; });

    SECTION("Creation")
    {
        Item item = Tests::makeItem("id");
        item.setValue("bug_number", "22");
        Tests::storeItem(storage, std::move(item));

        boost::optional<int> exitCode = cmd->run(*prj, { "id" });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        REQUIRE(boost::starts_with(split(out.str(), '\n')[0],
                                   "bug_number created:"));
        REQUIRE(err.str() == std::string());
    }

    SECTION("Removal")
    {
        Item item = Tests::makeItem("id");
        item.setValue("bug_number", "22");
        item.setValue("bug_number", "");
        Tests::storeItem(storage, std::move(item));

        boost::optional<int> exitCode = cmd->run(*prj, { "id" });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        REQUIRE(boost::starts_with(split(out.str(), '\n')[1],
                                   "bug_number deleted"));
        REQUIRE(err.str() == std::string());
    }

    SECTION("Changing")
    {
        Item item = Tests::makeItem("id");
        item.setValue("bug_number", "22");
        item.setValue("bug_number", "21");
        Tests::storeItem(storage, std::move(item));

        boost::optional<int> exitCode = cmd->run(*prj, { "id" });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        REQUIRE(boost::starts_with(split(out.str(), '\n')[1],
                                   "bug_number changed"));
        REQUIRE(err.str() == std::string());
    }
}

TEST_CASE("Completion of id for log", "[cmds][log][completion]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("log");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->complete(*prj, { "i" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut = "id\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Completion of field name for log", "[cmds][log][completion]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("log");

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
