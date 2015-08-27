// Copyright (C) 2015 xaizek <xaizek@openmailbox.org>
//
// This file is part of scribe.
//
// scribe is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// scribe is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with scribe.  If not, see <http://www.gnu.org/licenses/>.

#include "Command.hpp"

#include <iostream>
#include <string>
#include <utility>

Command::Command(std::string name, std::string descr, std::string help)
    : name(std::move(name)), descr(std::move(descr)), help(std::move(help))
{
}

std::string
Command::getName() const
{
    return name;
}

std::string
Command::getDescr() const
{
    return descr;
}

std::string
Command::getHelp() const
{
    return help;
}

boost::optional<int>
Command::run(Scribe &, const std::vector<std::string> &)
{
    return {};
}

boost::optional<int>
Command::run(Project &, const std::vector<std::string> &)
{
    return {};
}

boost::optional<int>
Command::complete(Scribe &, const std::vector<std::string> &)
{
    return {};
}

boost::optional<int>
Command::complete(Project &, const std::vector<std::string> &)
{
    return {};
}

std::ostream &
Command::out()
{
    return std::cout;
}

std::ostream &
Command::err()
{
    return std::cerr << getName() << ": ";
}
