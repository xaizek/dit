#include "Command.hpp"

#include <string>
#include <utility>

Command::Command(std::string name, std::string descr, std::string help)
    : name(std::move(name))
    , descr(std::move(descr))
    , help(std::move(help))
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
