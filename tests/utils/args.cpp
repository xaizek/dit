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

#include <string>
#include <vector>

#include "utils/args.hpp"

TEST_CASE("Empty input", "[utils][args]")
{
    REQUIRE(breakIntoArgs(std::string()).empty());
    REQUIRE(breakIntoArgs("        ").empty());
}

TEST_CASE("Benign input", "[utils][args]")
{
    std::vector<std::string> args = breakIntoArgs("a b c");
    REQUIRE(args == (std::vector<std::string>{ "a", "b", "c" }));
}

TEST_CASE("Leading whitespace", "[utils][args]")
{
    std::vector<std::string> args = breakIntoArgs(" a b c");
    REQUIRE(args == (std::vector<std::string>{ "a", "b", "c" }));
}

TEST_CASE("Extra whitespace", "[utils][args]")
{
    std::vector<std::string> args = breakIntoArgs("a   b   c");
    REQUIRE(args == (std::vector<std::string>{ "a", "b", "c" }));
}

TEST_CASE("Trailing whitespace", "[utils][args]")
{
    std::vector<std::string> args = breakIntoArgs("a b c ");
    REQUIRE(args == (std::vector<std::string>{ "a", "b", "c" }));
}

TEST_CASE("Whitespace everywhere", "[utils][args]")
{
    std::vector<std::string> args = breakIntoArgs("  a    b  c ");
    REQUIRE(args == (std::vector<std::string>{ "a", "b", "c" }));
}

TEST_CASE("Throws exception on unclosed bracket", "[utils][args]")
{
    REQUIRE_THROWS_AS(breakIntoArgs("'broken"), boost::escaped_list_error);
    REQUIRE_THROWS_AS(breakIntoArgs("\"broken"), boost::escaped_list_error);
    REQUIRE_THROWS_AS(breakIntoArgs("broken'"), boost::escaped_list_error);
    REQUIRE_THROWS_AS(breakIntoArgs("broken\""), boost::escaped_list_error);
}

TEST_CASE("Escaping works", "[utils][args]")
{
    std::vector<std::string> args = breakIntoArgs(R"(  "\n"  " \" "  \\ )");
    REQUIRE(args == (std::vector<std::string>{ "\n", " \" ", "\\" }));
}

TEST_CASE("Throws exception on wrong escaping", "[utils][args]")
{
    REQUIRE_THROWS_AS(breakIntoArgs("broken\\"), boost::escaped_list_error);
    REQUIRE_THROWS_AS(breakIntoArgs("broken\\x"), boost::escaped_list_error);
}
