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

#include <boost/property_tree/ptree.hpp>

#include "utils/propsRange.hpp"

TEST_CASE("Empty property tree is handled correctly", "[utils][props-range]")
{
    boost::property_tree::ptree props;

    bool hasItems = false;
    for (const std::string &propName : propsRange(props)) {
        static_cast<void>(propName);
        hasItems = true;
    }
    REQUIRE(!hasItems);
}
