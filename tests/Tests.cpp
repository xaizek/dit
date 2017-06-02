// Copyright (C) 2015 xaizek <xaizek@openmailbox.org>
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

#include "Tests.hpp"

#include <ctime>

#include <functional>
#include <string>
#include <utility>

#include "utils/memory.hpp"
#include "Change.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"
#include "decoration.hpp"

void
Tests::disableDecorations()
{
    decor::disableDecorations({});
}

void
Tests::setStreams(std::ostream &out, std::ostream &err)
{
    Command::setStreams(out, err, {});
}

void
Tests::setTimeSource(std::function<std::time_t()> getTime)
{
    Item::setTimeSource(getTime, {});
}

void
Tests::resetTimeSource()
{
    Item::setTimeSource({}, {});
}

std::unique_ptr<Project>
Tests::makeProject()
{
    return std::unique_ptr<Project>(new Project("/fake/project/root/for/tests",
                                                {}, {}));
}

Item
Tests::makeItem(std::string id)
{
    static Project dummyProject("/fake/project/root/for/tests", {}, {});
    static Storage dummyStorage(dummyProject);

    return Item(dummyStorage, std::move(id), {});
}

void
Tests::storeItem(Storage &storage, Item item)
{
    storage.put(std::move(item), {});
}

bool
Tests::configIsModified(Config &config)
{
    return config.isModified({});
}

void
Tests::removeCmd(const std::string &name)
{
    Commands::remove(name, {});
}
