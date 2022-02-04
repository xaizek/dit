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

#include <boost/filesystem/operations.hpp>

#include <stdexcept>

#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"

#include "Tests.hpp"

namespace fs = boost::filesystem;

TEST_CASE("Existing items are loaded", "[storage]")
{
    Project prj("tests/data/dit/projects/first");
    Storage &storage = prj.getStorage();

    REQUIRE(storage.list().size() == 2U);
    REQUIRE(storage.get("ehc").getValue("title") == "the title");
}

TEST_CASE("Items are created, stored and then loaded", "[storage]")
{
    std::string id;

    try {

        Project::init("tests/data/dit/projects/tmp");

        {
            Project prj("tests/data/dit/projects/tmp");
            Storage &storage = prj.getStorage();
            REQUIRE(storage.list().size() == 0U);

            // Item created here is empty, but it still must be saved.
            id = storage.create().getId();
            prj.save();
        }

        {
            Project prj("tests/data/dit/projects/tmp");
            Storage &storage = prj.getStorage();
            REQUIRE(storage.getIdGenerator().size() == 1U);
            REQUIRE(storage.list().size() == 1U);

            REQUIRE_NOTHROW(storage.get(id));
            prj.save();
        }

    } catch (...) {
        fs::remove_all("tests/data/dit/projects/tmp");
        throw;
    }

    fs::remove_all("tests/data/dit/projects/tmp");
}

TEST_CASE("Storage throws on save if project removed", "[storage]")
{
    std::string id;

    try {

        Project::init("tests/data/dit/projects/tmp");

        {
            Project prj("tests/data/dit/projects/tmp");
            Storage &storage = prj.getStorage();
            REQUIRE(storage.list().size() == 0U);

            Item &item = storage.create();
            item.setValue("title", "title value");
            id = item.getId();
            prj.save();
        }

        {
            Project prj("tests/data/dit/projects/tmp");
            Storage &storage = prj.getStorage();
            REQUIRE(storage.getIdGenerator().size() == 1U);
            REQUIRE(storage.list().size() == 1U);

            Item &item = storage.get(id);
            fs::remove_all("tests/data/dit/projects/tmp");

            REQUIRE_THROWS_AS(item.getValue("title"),
                              const std::runtime_error &);
        }

    } catch (...) {
        fs::remove_all("tests/data/dit/projects/tmp");
        throw;
    }

    fs::remove_all("tests/data/dit/projects/tmp");
}
