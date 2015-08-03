#include "Scribe.hpp"

#include <cassert>
#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

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
        cmd = DEFAULT_CMD;
    } else {
        cmd = args[0];
        args.erase(args.begin());
    }
}

int
Scribe::run()
{
    std::cout << "Command: " << cmd << std::endl;

    std::cout << "Arguments:" << std::endl;
    std::copy(args.cbegin(), args.cend(),
              std::ostream_iterator<std::string>(std::cout, "\n"));
    return EXIT_SUCCESS;
}
