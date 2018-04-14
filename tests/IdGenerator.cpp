// Copyright (C) 2015 xaizek <xaizek@posteo.net>
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

#include <algorithm>

#include "Config.hpp"
#include "IdGenerator.hpp"

#include "Tests.hpp"

TEST_CASE("Config is populated with data on init", "[ids]")
{
    Config config("config");

    REQUIRE(!Tests::configIsModified(config));
    IdGenerator::init(config, "1234567890");
    REQUIRE(Tests::configIsModified(config));
}

TEST_CASE("Config is not updated with data on save if not needed", "[ids]")
{
    Config parent("parent");
    Config child("child", &parent);

    IdGenerator::init(parent, "1234567890");
    IdGenerator idGenerator(child);

    REQUIRE(!Tests::configIsModified(child));
    idGenerator.save();
    REQUIRE(!Tests::configIsModified(child));
}

TEST_CASE("Config is updated with data on save after change", "[ids]")
{
    Config parent("parent");
    Config child("child", &parent);

    IdGenerator::init(parent, "1234567890");
    IdGenerator idGenerator(child);

    idGenerator.advanceId();
    REQUIRE(!Tests::configIsModified(child));
    idGenerator.save();
    REQUIRE(Tests::configIsModified(child));
}

TEST_CASE("IDs can not be exhausted", "[ids]")
{
    Config parent("parent");
    Config child("child", &parent);

    IdGenerator::init(parent,
                      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    IdGenerator idGenerator(child);

    REQUIRE(idGenerator.getId().length() == 3U);

    for (int i = 0; i < 52*52*52; ++i) {
        idGenerator.advanceId();
    }

    REQUIRE(idGenerator.getId().length() == 4U);
}

TEST_CASE("All IDs are distinct", "[ids]")
{
    Config parent("parent");
    Config child("child", &parent);

    IdGenerator::init(parent, "1234567890");
    IdGenerator idGenerator(child);

    std::vector<std::string> ids;
    ids.reserve(10*10*10*10*10);
    for (int i = 0; i < 10*10*10*10*10; ++i) {
        ids.push_back(idGenerator.getId());
        idGenerator.advanceId();
    }

    std::sort(ids.begin(), ids.end());
    std::unique(ids.begin(), ids.end());

    REQUIRE(ids.size() == 10*10*10*10*10);
}

TEST_CASE("Load and store of extended ids", "[ids]")
{
    Config config("config");

    IdGenerator::init(config, "1234567890");
    IdGenerator idGenerator(config);

    for (int i = 0; i < 10*10*10*10*10; ++i) {
        idGenerator.advanceId();
    }

    idGenerator.save();
    IdGenerator idGenerator2(config);

    REQUIRE(idGenerator2.getId().length() == 5U);
}
