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

#include <vector>

#include "utils/containers.hpp"

using container = std::vector<int>;

TEST_CASE("Empty input yields empty output.", "[utils][containers][span]")
{
    auto split = span(container{}, [](int){ return false; });
    REQUIRE(split.first == container{});
    REQUIRE(split.second == container{});
}

TEST_CASE("Left part takes all.", "[utils][containers][span]")
{
    auto split = span(container{1, 2, 3}, [](int){ return true; });
    REQUIRE(split.first == (container{1, 2, 3}));
    REQUIRE(split.second == container{});
}

TEST_CASE("Right part takes all.", "[utils][containers][span]")
{
    auto split = span(container{1, 2, 3}, [](int){ return false; });
    REQUIRE(split.first == container{});
    REQUIRE(split.second == (container{1, 2, 3}));
}

TEST_CASE("Normal case splits container.", "[utils][containers][span]")
{
    auto split = span(container{1, 8, 3, 9}, [](int a){ return a < 5; });
    REQUIRE(split.first == container{1});
    REQUIRE(split.second == (container{8, 3, 9}));
}
