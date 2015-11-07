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

#include "Config.hpp"
#include "Project.hpp"
#include "Scribe.hpp"

#include "Tests.hpp"

TEST_CASE("Completion for sub-commands works", "[app][completion]")
{
    Tests::disableDecorations();

    const char *const args[] = { "app" };
    char xdg_env[] = "XDG_CONFIG_HOME=";
    char home_env[] = "HOME=.";

    putenv(xdg_env);
    putenv(home_env);

    Scribe scribe(1, args);

    Project prj = Tests::makeProject();

    Config &cfg = prj.getConfig(false);
    cfg.set("ui.ls", "ls-value");
    cfg.set("ui.show", "show-value");

    std::ostringstream out;
    std::ostringstream err;
    Tests::setStreams(out, err);

    boost::optional<int> exitCode = scribe.complete(prj, { "config", "ui." });
    REQUIRE(exitCode);
    REQUIRE(*exitCode == EXIT_SUCCESS);

    const std::string expectedOut =
        "ui.ls:\n"
        "ui.show:\n";
    REQUIRE(out.str() == expectedOut);
    REQUIRE(err.str() == std::string());
}
