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

#include <cstdlib>

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "utils/strings.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Config.hpp"
#include "Project.hpp"

namespace {

/**
 * @brief Implementation of "config" command, which gets/sets configuration.
 */
class ConfigCmd : public Command
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    ConfigCmd();

public:
    /**
     * @copydoc Command::run()
     */
    virtual boost::optional<int> run(
        Project &project,
        const std::vector<std::string> &args) override;

private:
    int printAllValues(Config &config);
    void printKey(Config &config, const std::string &key);
};

REGISTER_COMMAND(ConfigCmd);

}

ConfigCmd::ConfigCmd()
    : Command("config", "read/update configuration",
              "Usage: config [key[=val]...]")
{
}

boost::optional<int>
ConfigCmd::run(Project &project, const std::vector<std::string> &args)
{
    Config &config = project.getConfig();

    if (args.empty()) {
        return printAllValues(config);
    }

    for (const std::string &a : args) {
        bool set;
        std::string key, value;
        try {
            std::tie(key, value) = splitAt(a, '=');
            set = true;
        } catch (std::runtime_error &) {
            set = false;
            key = a;
        }

        if (key.empty() || key[0] == '!') {
            std::cout << "Wrong key name: '" << key << "'\n";
            continue;
        }

        if (set) {
            config.set(key, value);
        } else {
            printKey(config, key);
        }
    }
    std::cout.flush();

    return EXIT_SUCCESS;
}

/**
 * @brief Lists configuration keys along with their values.
 *
 * Builtin keys are not listed.
 *
 * @param config Configuration to read keys from.
 *
 * @returns Error code.
 */
int
ConfigCmd::printAllValues(Config &config)
{
    for (const std::string &key : config.list()) {
        printKey(config, key);
    }
    return EXIT_FAILURE;
}

/**
 * @brief Prints single key-value pair.
 *
 * @param config Configuration.
 * @param key Name of key to print.
 */
void
ConfigCmd::printKey(Config &config, const std::string &key)
{
    std::cout << key << " = " << config.get(key, "<not set>") << '\n';
}
