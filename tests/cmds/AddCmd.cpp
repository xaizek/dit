// Copyright (C) 2015 xaizek <xaizek@openmailbox.org>
//
// This file is part of scribe.
//
// scribe is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// scribe is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with scribe.  If not, see <http://www.gnu.org/licenses/>.

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
    Tests::disableDecorations();

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
