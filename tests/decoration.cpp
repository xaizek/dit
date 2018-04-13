// Copyright (C) 2016 xaizek <xaizek@posteo.net>
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

#include <sstream>

#include "decoration.hpp"

#include "Tests.hpp"

TEST_CASE("Decorations do nothing, when disabled", "[decorations]")
{
    Tests::disableDecorations();

    std::ostringstream oss;

    using namespace decor;

    oss << none << bold << inv << def;

    oss << black_fg << red_fg << green_fg << yellow_fg
        << blue_fg << magenta_fg << cyan_fg << white_fg;

    oss << black_bg << red_bg << green_bg << yellow_bg
        << blue_bg << magenta_bg << cyan_bg << white_bg;

    REQUIRE(oss.str().empty());
}
