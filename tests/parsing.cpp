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

#include <string>
#include <vector>

#include "parsing.hpp"

TEST_CASE("Assign-like pairs with invalid keys are ignored.",
          "[parsing][colon-style-pairs]")
{
    const std::vector<std::string> input = {
        "url:",
        "http://some.url/?show=#",
    };

    const std::vector<std::string> parsed = parsePairedArgs(input);

    REQUIRE(parsed.size() == 1);
    REQUIRE(parsed[0] == "url=http://some.url/?show=#");
}

TEST_CASE("Trailing colon-assignment", "[parsing][colon-style-pairs]")
{
    const std::vector<std::string> input = { "title:", "title", "comment:" };

    const std::vector<std::string> parsed = parsePairedArgs(input);

    REQUIRE(parsed.size() == 2);
    REQUIRE(parsed[0] == "title=title");
    REQUIRE(parsed[1] == "comment=");
}
