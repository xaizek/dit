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
#include <stdexcept>
#include <vector>

#include "Change.hpp"
#include "file_format.hpp"

static inline bool operator==(const Change &lhs, const Change &rhs);

TEST_CASE("Serialization of empty changeset.", "[file-format]")
{
    std::vector<Change> o;
    std::vector<Change> d;

    std::stringstream ss;
    ss << o;
    ss >> d;

    REQUIRE(d == o);
}

TEST_CASE("Deserialization of broken stream errors.", "[file-format]")
{
    std::vector<Change> d;

    std::stringstream ss;
    ss << "bla=bla\n";

    REQUIRE_THROWS_AS(ss >> d, std::runtime_error);
}

TEST_CASE("Deserialized objects are identical to original", "[file-format]")
{
    std::vector<Change> o {
        { 123, "key1", "value1" },
        { 124, "key2", "value2" },
    };
    std::vector<Change> d;

    std::stringstream ss;
    ss << o;
    ss >> d;

    REQUIRE(d == o);
}

TEST_CASE("Deserialization throws on empty string", "[file-format]")
{
    std::vector<Change> o {
        { 123, "key1", "value1" },
    };
    std::vector<Change> d;

    std::stringstream ss;
    ss << '\n' << o;
    REQUIRE_THROWS_AS(ss >> d, std::runtime_error);
}

static inline bool
operator==(const Change &lhs, const Change &rhs)
{
    return lhs.getTimestamp() == rhs.getTimestamp()
        && lhs.getKey() == rhs.getKey()
        && lhs.getValue() == rhs.getValue();
}
