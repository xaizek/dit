// Copyright (C) 2015 xaizek <xaizek@posteo.net>
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
#include "Dit.hpp"
#include "Project.hpp"

namespace fs = boost::filesystem;

/**
 * @brief Usage message for "complete" command.
 */
const char *const USAGE = "Usage: complete <regular args>";

namespace {

/**
 * @brief Implementation of "complete" command, which helps with completion.
 */
class CompleteCmd : public AutoRegisteredCommand<CompleteCmd>
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
        Dit &dit,
        const std::vector<std::string> &args) override;
};

}

CompleteCmd::CompleteCmd()
    : parent("complete", "perform command-line completion", USAGE)
{
}

boost::optional<int>
CompleteCmd::run(Dit &dit, const std::vector<std::string> &args)
{
    std::ostringstream estream;
    int exitCode = dit.complete(args, out(), estream);
    if (!estream.str().empty()) {
        // Calling err() has side effects, so don't call unless error occurred.
        err() << estream.str();
    }
    return exitCode;
}
