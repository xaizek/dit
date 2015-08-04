#include <cstdlib>

#include <iostream>
#include <string>
#include <vector>

#include "Command.hpp"
#include "Commands.hpp"

namespace {

/**
 * @brief Implementation of "ls" command, which lists items.
 */
class Ls : public Command
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    Ls();

public:
    virtual int run(const std::vector<std::string> &args) override;
};

REGISTER_COMMAND(Ls);

}

Ls::Ls() : Command("ls", "lists items", "Usage: ls")
{
}

int
Ls::run(const std::vector<std::string> &args)
{
    static_cast<void>(args);

    std::cout << "In ls command." << std::endl;
    return EXIT_SUCCESS;
}
