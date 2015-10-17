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

#include <sstream>
#include <string>

#include "Change.hpp"
#include "Item.hpp"
#include "ItemTable.hpp"

#include "Tests.hpp"

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

TEST_CASE("Longer column gets shortened first.", "[item-table][sizing]")
{
    Item item = Tests::makeItem("id");
    item.setValue("title", "title");

    ItemTable table("_id|title", std::string(), std::string(), 8);
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

    ItemTable table("_id|title", std::string(), std::string(), 0);
    table.append(item);

    std::ostringstream oss;
    table.print(oss);

    REQUIRE(oss.str() == std::string());
}
