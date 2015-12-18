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

#include "Change.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Config.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"

#include "Tests.hpp"

TEST_CASE("New item is added successfully", "[cmds][add]")
{
    Command *const cmd = Commands::get("add");
    std::unique_ptr<Project> prj = Tests::makeProject();
    Config &cfg = prj->getConfig(false);

    cfg.set("!ids.sequences.first",
            "BWlupmoqXJwfSsRzgFvMEDtUOjTnePYrGbxANhVLcIQyKkidHZCa");
    cfg.set("!ids.sequences.second",
            "MYgRHkGujvoawZnJhLdNciFxPKmACQEeWUtDBTXVOzIpSyrlqfsb");
    cfg.set("!ids.sequences.third",
            "PIxFYDACivrWKVpSLMRmzuTHNGwktZOcBXldJjhygnQbEeqUfoas");
    cfg.set("!ids.count", "64");
    cfg.set("!ids.next", "fYP");

    std::ostringstream out;
    std::ostringstream err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->run(*prj, { "title:", "title" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    REQUIRE(out.str() == "Created item: fYP\n");
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Completion of first key name on addition", "[cmds][add][completion]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("add");

    std::ostringstream out;
    std::ostringstream err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->complete(*prj, { "ti" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut =
        "title:\n"
        "bug_number:\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Completion of next key name on addition", "[cmds][add][completion]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("add");

    std::ostringstream out;
    std::ostringstream err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->complete(*prj, { "title:", "t", "b" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut = "bug_number:\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}

TEST_CASE("Completion of a value on addition", "[cmds][add][completion]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");

    Tests::storeItem(storage, std::move(item));

    Command *const cmd = Commands::get("add");

    std::ostringstream out;
    std::ostringstream err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = cmd->complete(*prj, { "title=" });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut = "title\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}
