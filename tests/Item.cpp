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

#include <boost/filesystem/operations.hpp>

#include <ctime>

#include <sstream>
#include <stdexcept>
#include <string>

#include "Change.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"

#include "Tests.hpp"

namespace fs = boost::filesystem;

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
        REQUIRE_THROWS_AS(item.getValue("!@@#$%^"), const std::runtime_error &);
    }

    SECTION("On set().")
    {
        REQUIRE_THROWS_AS(item.setValue("!@@#$%^", "b"),
                          const std::runtime_error &);
    }
}

TEST_CASE("Setting field sets modified flag.", "[item]")
{
    Item item = Tests::makeItem("aaa");

    REQUIRE(!item.wasChanged());
    item.setValue("bla", "b");
    REQUIRE(item.wasChanged());
}

TEST_CASE("Setting empty field to the same value doesn't count as change.",
          "[item]")
{
    Item item = Tests::makeItem("id");

    REQUIRE(!item.wasChanged());
    item.setValue("title", "");
    REQUIRE(!item.wasChanged());
}

TEST_CASE("Reverted change cancels out the wrong one for empty field", "[item]")
{
    Item item = Tests::makeItem("id");

    item.setValue("title", "new title");
    REQUIRE(item.getChanges().size() == 1U);
    item.setValue("title", "");
    REQUIRE(item.getChanges().size() == 0U);
}

TEST_CASE("Setting field to the same value doesn't count as change.", "[item]")
{
    Item item = Tests::makeItem("id");

    item.setValue("title", "the title");
    REQUIRE(item.getChanges().size() == 1U);
    item.setValue("title", "the title");
    REQUIRE(item.getChanges().size() == 1U);
}

TEST_CASE("Reverted change cancels out the wrong one", "[item]")
{
    Item item = Tests::makeItem("id");

    {
        // The first change should be an old one.
        MockTimeSource timeMock([](){ return 0; });
        item.setValue("title", "old title");
        REQUIRE(item.getChanges().size() == 1U);
    }

    item.setValue("title", "new title");
    REQUIRE(item.getChanges().size() == 2U);
    item.setValue("title", "old title");
    REQUIRE(item.getChanges().size() == 1U);
}

TEST_CASE("Items with broken timestamps fail to load.", "[item][load]")
{
    std::time_t t = std::time(nullptr);
    MockTimeSource timeMock([&t](){ return t--; });

    try {

        Project::init("tests/data/dit/projects/tmp");
        std::string id;

        {
            Project prj("tests/data/dit/projects/tmp");
            Storage &storage = prj.getStorage();
            REQUIRE(storage.list().size() == 0U);

            Item &item = storage.create();
            item.setValue("a", "b");
            item.setValue("c", "d");
            id = item.getId();
            prj.save();
        }

        {
            Project prj("tests/data/dit/projects/tmp");
            Storage &storage = prj.getStorage();
            REQUIRE_THROWS_AS(storage.get(id).getValue("c"),
                              const std::logic_error &);
        }

    } catch (...) {
        fs::remove_all("tests/data/dit/projects/tmp");
        throw;
    }

    fs::remove_all("tests/data/dit/projects/tmp");
}
