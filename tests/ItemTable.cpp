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

#include <sstream>
#include <stdexcept>
#include <string>

#include "Change.hpp"
#include "Item.hpp"
#include "ItemTable.hpp"

#include "Tests.hpp"

TEST_CASE("Throws on wrong specification.", "[item-table][format]")
{
    REQUIRE_THROWS_AS(ItemTable(std::string(), "this is not valid",
                                std::string(), 80),
                      const std::runtime_error &);
}

TEST_CASE("No columns result in no output.", "[item-table][format]")
{
    Item item = Tests::makeItem("id");
    item.setValue("title", "title");

    ItemTable table(std::string(), std::string(), std::string(), 80);
    table.append(item);

    std::ostringstream oss;
    table.print(oss);

    REQUIRE(oss.str() == std::string());
}

TEST_CASE("Invisible columns turn into dots.", "[item-table][sizing]")
{
    Item item = Tests::makeItem("id");
    item.setValue("title", "title");

    ItemTable table("title", std::string(), std::string(), 2);
    table.append(item);

    std::ostringstream oss;
    table.print(oss);

    const std::string expected = "..\n"
                                 "..\n";
    REQUIRE(oss.str() == expected);
}

TEST_CASE("Longer column gets shortened first.", "[item-table][sizing]")
{
    Item item = Tests::makeItem("id");
    item.setValue("title", "title");

    ItemTable table("_id,title", std::string(), std::string(), 8);
    table.append(item);

    std::ostringstream oss;
    table.print(oss);

    const std::string expected = "ID  T...\n"
                                 "id  t...\n";
    REQUIRE(oss.str() == expected);
}

TEST_CASE("Zero terminal size results in no output.", "[item-table][sizing]")
{
    Item item = Tests::makeItem("id");
    item.setValue("title", "title");

    ItemTable table("_id,title", std::string(), std::string(), 0);
    table.append(item);

    std::ostringstream oss;
    table.print(oss);

    REQUIRE(oss.str() == std::string());
}

TEST_CASE("Sorting is performed.", "[item-table][sorting]")
{
    Item itemA = Tests::makeItem("aaa");
    Item itemB = Tests::makeItem("bbb");
    Item itemC = Tests::makeItem("ccc");

    ItemTable table("_id", "inv _id==aaa", std::string("_id"), 80);
    table.append(itemC);
    table.append(itemA);
    table.append(itemB);

    std::ostringstream oss;
    table.print(oss);

    const std::string expected =
        "ID \n"
        "aaa\n"
        "bbb\n"
        "ccc\n";
    REQUIRE(oss.str() == expected);
}
