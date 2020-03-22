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

TEST_CASE("Check finds errors and validates correctly", "[cmds][check]")
{
    Tests::disableDecorations();

    std::unique_ptr<Project> prj = Tests::makeProject();

    Config &cfg = prj->getConfig(false);
    cfg.set("!ids.sequences.alphabet",
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    cfg.set("!ids.sequences.count", "3");
    cfg.set("!ids.sequences.0",
            "BWlupmoqXJwfSsRzgFvMEDtUOjTnePYrGbxANhVLcIQyKkidHZCa");
    cfg.set("!ids.sequences.1",
            "MYgRHkGujvoawZnJhLdNciFxPKmACQEeWUtDBTXVOzIpSyrlqfsb");
    cfg.set("!ids.sequences.2",
            "PIxFYDACivrWKVpSLMRmzuTHNGwktZOcBXldJjhygnQbEeqUfoas");
    cfg.set("!ids.count", "0");
    cfg.set("!ids.total", "0");
    cfg.set("!ids.next", "fYP");

    Storage &storage = prj->getStorage();

    std::ostringstream out, err;
    Tests::setStreams(out, err);

    Command *const cmd = Commands::get("check");
    boost::optional<int> exitCode;

    SECTION("Single item with correct id")
    {
        cfg.set("!ids.count", "1");
        cfg.set("!ids.total", "1");
        cfg.set("!ids.next", "WYP");

        Tests::storeItem(storage, Tests::makeItem("BMP"));

        exitCode = cmd->run(*prj, { });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_SUCCESS);

        REQUIRE(out.str() == std::string());
    }

    SECTION("Not enough actual items")
    {
        cfg.set("!ids.count", "1");
        cfg.set("!ids.total", "1");

        exitCode = cmd->run(*prj, { });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_FAILURE);

        REQUIRE(out.str() != std::string());
    }

    SECTION("Item with incorrect id")
    {
        cfg.set("!ids.count", "1");
        cfg.set("!ids.total", "1");
        cfg.set("!ids.next", "WYP");

        Tests::storeItem(storage, Tests::makeItem("BxP"));

        exitCode = cmd->run(*prj, { });
        REQUIRE(exitCode);
        REQUIRE(*exitCode == EXIT_FAILURE);

        REQUIRE(out.str() != std::string());
    }

    REQUIRE(err.str() == std::string());
}
