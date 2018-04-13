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

#include "Command.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <utility>

/**
 * @brief Stream for regular output.
 */
static std::reference_wrapper<std::ostream> out = std::cout;
/**
 * @brief Stream for error messages.
 */
static std::reference_wrapper<std::ostream> err = std::cerr;

Command::Command(std::string name, std::string descr, std::string help)
    : name(std::move(name)), descr(std::move(descr)), help(std::move(help))
{
}

Command::~Command()
{
    // Put destructor and virtual table here.
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
Command::run(Dit &, const std::vector<std::string> &)
{
    return {};
}

boost::optional<int>
Command::run(Project &, const std::vector<std::string> &)
{
    return {};
}

boost::optional<int>
Command::complete(Dit &, const std::vector<std::string> &)
{
    return {};
}

boost::optional<int>
Command::complete(Project &, const std::vector<std::string> &)
{
    return {};
}

void
Command::setStreams(std::ostream &out, std::ostream &err, pk<Tests>)
{
    ::out = out;
    ::err = err;
}

std::ostream &
Command::out()
{
    return ::out.get();
}

std::ostream &
Command::err()
{
    return ::err.get() << getName() << ": ";
}
