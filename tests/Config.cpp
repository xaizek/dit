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

#include "Config.hpp"

TEST_CASE("Absent values are taken from parent", "[config][parent-child]")
{
    Config parent("parent");
    parent.set("key", "value");

    Config child("child", &parent);

    SECTION("Without default value.")
    {
        REQUIRE(child.get("key", "v") == "value");
    }

    SECTION("With default value.")
    {
        REQUIRE(child.get("key") == "value");
    }
}

TEST_CASE("For unknown path empty list is returned", "[config][list]")
{
    Config cfg("cfg");

    REQUIRE(cfg.list("path") == std::vector<std::string>());
}
