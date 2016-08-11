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

#include <ctime>

#include <sstream>
#include <stdexcept>
#include <string>

#include "Change.hpp"
#include "Item.hpp"

#include "Tests.hpp"

TEST_CASE("_id pseudo field returns item id.", "[item][pseudo-field]")
{
    Item item = Tests::makeItem("aaa");
    REQUIRE(item.getValue("_id") == "aaa");
}

TEST_CASE("Builtin keys are not writable", "[item][pseudo-field]")
{
    std::string error;
    REQUIRE(!Item::isValidKeyName("_id", true, error));
    REQUIRE(error != std::string());
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
    MockTimeSource timeMock([&t](){ return t++; });

    Item itemA = Tests::makeItem("aaa");
    Item itemB = Tests::makeItem("bbb");

    itemA.setValue("title", "value");
    itemB.setValue("title", "value");

    REQUIRE(itemB.getValue("_created") > itemA.getValue("_created"));
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
    MockTimeSource timeMock([&t](){ return t++; });

    Item itemA = Tests::makeItem("aaa");
    Item itemB = Tests::makeItem("bbb");

    itemA.setValue("title", "value");
    itemB.setValue("title", "value");
    REQUIRE(itemB.getValue("_changed") > itemA.getValue("_changed"));
}

TEST_CASE("Empty values are not listed.", "[item]")
{
    std::time_t t = std::time(nullptr);
    MockTimeSource timeMock([&t](){ return t++; });

    Item item = Tests::makeItem("aaa");
    item.setValue("title", "some title");
    REQUIRE(item.listRecordNames() == std::set<std::string>{"title"});
    item.setValue("title", std::string());
    REQUIRE(item.listRecordNames() == std::set<std::string>{});
}

TEST_CASE("Wrongly named keys cause error.", "[item]")
{
    Item item = Tests::makeItem("aaa");

    SECTION("On get().")
    {
        REQUIRE_THROWS_AS(item.getValue("!@@#$%^"), std::runtime_error);
    }

    SECTION("On set().")
    {
        REQUIRE_THROWS_AS(item.setValue("!@@#$%^", "b"), std::runtime_error);
    }
}

TEST_CASE("Setting field sets modified flag.", "[item]")
{
    Item item = Tests::makeItem("aaa");

    REQUIRE(!item.wasChanged());
    item.setValue("bla", "b");
    REQUIRE(item.wasChanged());
}
