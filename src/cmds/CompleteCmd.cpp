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

#include <cassert>
#include <cstdlib>

#include <functional>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "Command.hpp"
#include "Commands.hpp"
#include "Config.hpp"
#include "Project.hpp"
#include "Scribe.hpp"

namespace fs = boost::filesystem;

/**
 * @brief Usage message for "ls" command.
 */
const char *const USAGE = "Usage: complete <regular args>";

namespace {

/**
 * @brief Implementation of "complete" command, which helps with completion.
 */
class CompleteCmd : public Command
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    CompleteCmd();

public:
    /**
     * @copydoc Command::run()
     */
    virtual boost::optional<int> run(
        Scribe &scribe,
        const std::vector<std::string> &args) override;
};

REGISTER_COMMAND(CompleteCmd);

}

CompleteCmd::CompleteCmd()
    : Command("complete", "command-line completion helper", USAGE)
{
}

boost::optional<int>
CompleteCmd::run(Scribe &scribe, const std::vector<std::string> &args)
{
    std::ostringstream estream;
    int exitCode = scribe.complete({ args.begin() + 1, args.end() },
                                   out(), estream);
    if (!estream.str().empty()) {
        err() << estream.str();
    }
    return exitCode;
}
