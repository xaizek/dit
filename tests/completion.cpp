// Copyright (C) 2016 xaizek <xaizek@posteo.net>
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

#include "Change.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"
#include "completion.hpp"

#include "Tests.hpp"

TEST_CASE("Present key names are absent from completion", "[completion]")
{
    std::unique_ptr<Project> prj = Tests::makeProject();
    Storage &storage = prj->getStorage();

    Item item = Tests::makeItem("id");
    item.setValue("title", "title");
    item.setValue("bug_number", "22");
    item.setValue("author", "me");

    Tests::storeItem(storage, std::move(item));

    std::ostringstream oss;

    const std::vector<std::string> args = { "title=new", "author:", "notme" };
    REQUIRE(completeKeys(storage, oss, args) == EXIT_SUCCESS);

    const std::string expectedOut = "bug_number:\n";
    REQUIRE(oss.str() == expectedOut);
}
