// Copyright (C) 2016 xaizek <xaizek@openmailbox.org>
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

#include <stdexcept>
#include <string>

#include "utils/strings.hpp"
#include "Change.hpp"
#include "Item.hpp"
#include "ItemFilter.hpp"

#include "Tests.hpp"

TEST_CASE("Throws on wrong syntax", "[item-filter]")
{
    REQUIRE_THROWS_AS(ItemFilter({ "wrong expr" }), std::runtime_error);
}

TEST_CASE("Error messages add up", "[item-filter]")
{
    Item item = Tests::makeItem("id");
    item.setValue("title", "title");

    ItemFilter filter({ "_id==notid", "title!=title", "_id!/id", "title/xy" });

    std::string error;
    auto accessor = [&item](const std::string &f) { return item.getValue(f); };
    REQUIRE(!filter.passes(accessor, error));

    REQUIRE(split(error, '\n').size() == 4);
}
