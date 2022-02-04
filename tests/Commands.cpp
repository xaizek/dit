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

#include <cstddef>

#include <memory>
#include <stdexcept>
#include <utility>

#include "utils/memory.hpp"
#include "Command.hpp"
#include "Commands.hpp"

TEST_CASE("Get doesn't have side effects", "[commands]")
{
    const std::size_t commandCount = Commands::list().size();

    REQUIRE(Commands::get("doesn't exist") == nullptr);
    REQUIRE(Commands::list().size() == commandCount);
}

TEST_CASE("Commands must have unique names", "[commands]")
{
    class Cmd : public Command {
    public:
        Cmd(std::string name, std::string descr, std::string help)
            : Command(name, descr, help) { }
    };

    Command *const cmd = Commands::get("log");
    REQUIRE(cmd != nullptr);

    auto fakeCmd = make_unique<Cmd>("log", std::string(), std::string());
    REQUIRE_THROWS_AS(Commands::add(std::move(fakeCmd)),
                      const std::runtime_error &);
}
