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

#include <memory>
#include <sstream>
#include <stdexcept>

#include "Change.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"

#include "Tests.hpp"

TEST_CASE("Export fails on wrong invocation", "[cmds][export][invocation]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Command *const cmd = Commands::get("export");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode;

    SECTION("No arguments")
    {
        exitCode = cmd->run(*prj, {});
    }

    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_FAILURE);

    REQUIRE(out.str() == std::string());
    REQUIRE(err.str() != std::string());
}

TEST_CASE("Completion of export", "[cmds][export][completion]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("export");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode;
    std::string expectedOut;

    SECTION("First argument is not completed")
    {
        exitCode = cmd->complete(*prj, { "ti" });
    }

    SECTION("Second argument is completed")
    {
        exitCode = cmd->complete(*prj, { "cmd", "ti" });
        expectedOut =
            "bug_number\n"
            "title\n";
    }

    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Item exporting", "[cmds][export]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "This's a title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("export");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    SECTION("No operation run")
    {
        boost::optional<int> exitCode = cmd->run(*prj, { ":" });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);
    }

    SECTION("Failed run")
    {
        REQUIRE_THROWS_AS(cmd->run(*prj, { "no-such-cmd" }),
                          const std::runtime_error &);
    }

    REQUIRE(out.str() == std::string());
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Stdout destination", "[cmds][export]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "This's a title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("export");

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->run(*prj, { "-" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    // There must be two trailing zeroes.
    const char expectedOut[] = "_id=id\0bug_number=22\0title=This's a title\0";
    REQUIRE(out.str() == std::string(std::begin(expectedOut),
                                     std::end(expectedOut)));
    REQUIRE(err.str() == std::string());
}
