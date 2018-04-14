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

#include <cstdlib>

#include "integration.hpp"

TEST_CASE("On failure curent value is returned.", "[integration]")
{
    static char editor_env[] = "EDITOR=wrong-command >>";
    putenv(editor_env);

    boost::optional<std::string> val = editValue("key", "-", "current");
    REQUIRE(val);
    REQUIRE(*val == "current");
}
