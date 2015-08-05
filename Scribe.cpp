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

#include "Scribe.hpp"

#include <cassert>
#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "Command.hpp"
#include "Commands.hpp"
#include "Item.hpp"
#include "Storage.hpp"

/**
 * @brief Default command name used when run without arguments.
 */
static std::string DEFAULT_CMD = "ls";

Scribe::Scribe(int argc, const char *const argv[])
{
    if (argc < 1) {
        throw std::runtime_error("Broken argument list.");
    }

    appName = argv[0];
    std::copy(argv + 1, argv + argc, std::back_inserter(args));

    if (args.empty()) {
        cmdName = DEFAULT_CMD;
    } else {
        cmdName = args[0];
        args.erase(args.begin());
    }
}

int
Scribe::run()
{
    std::cout << "Command: " << cmdName << std::endl;

    std::cout << "Arguments:" << std::endl;
    std::copy(args.cbegin(), args.cend(),
              std::ostream_iterator<std::string>(std::cout, "\n"));

    Command *const cmd = Commands::get(cmdName);
    if (cmd == nullptr) {
        std::cerr << "Unknown command name: " << cmdName << std::endl;
        return EXIT_FAILURE;
    }

    Storage storage;
    cmd->run(storage, args);

    return EXIT_SUCCESS;
}
