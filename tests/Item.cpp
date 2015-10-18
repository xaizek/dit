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

#include <ctime>

#include <sstream>

#include "Change.hpp"
#include "Item.hpp"

#include "Tests.hpp"

TEST_CASE("_id pseudo field returns item id.", "[item][pseudo-field]")
{
    Item item = Tests::makeItem("aaa");
    REQUIRE(item.getValue("_id") == "aaa");
}

TEST_CASE("_created pseudo field is empty for empty item.",
          "[item][pseudo-field]")
{
    Item item = Tests::makeItem("aaa");
    REQUIRE(item.getValue("_created") == std::string());
}

TEST_CASE("_created pseudo field is updated correctly.", "[item][pseudo-field]")
{
    std::time_t t = std::time(nullptr);
    std::function<std::time_t()> timeSource = [&t](){ return t++; };
    Tests::setTimeSource(timeSource);

    Item itemA = Tests::makeItem("aaa");
    Item itemB = Tests::makeItem("bbb");

    itemA.setValue("title", "value");
    itemB.setValue("title", "value");

    REQUIRE(itemB.getValue("_created") > itemA.getValue("_created"));

    Tests::resetTimeSource();
}

TEST_CASE("_changed pseudo field is empty for empty item.",
          "[item][pseudo-field]")
{
    Item item = Tests::makeItem("aaa");
    REQUIRE(item.getValue("_changed") == std::string());
}

TEST_CASE("_changed pseudo field is updated correctly.", "[item][pseudo-field]")
{
    std::time_t t = std::time(nullptr);
    std::function<std::time_t()> timeSource = [&t](){ return t++; };
    Tests::setTimeSource(timeSource);

    Item itemA = Tests::makeItem("aaa");
    Item itemB = Tests::makeItem("bbb");

    itemA.setValue("title", "value");
    itemB.setValue("title", "value");
    REQUIRE(itemB.getValue("_changed") > itemA.getValue("_changed"));

    Tests::resetTimeSource();
}
