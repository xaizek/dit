#include <cstdlib>

#include <iostream>
#include <stdexcept>

#include "Scribe.hpp"

int
main(int argc, char *argv[])
{
    try {
        return Scribe(argc, argv).run();
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
