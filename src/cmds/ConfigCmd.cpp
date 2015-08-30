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

#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include <boost/program_options.hpp>

#include "utils/strings.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Config.hpp"
#include "Project.hpp"
#include "Scribe.hpp"
#include "decoration.hpp"

namespace po = boost::program_options;

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
        Scribe &scribe,
        const std::vector<std::string> &args) override;
    /**
     * @copydoc Command::run()
     */
    virtual boost::optional<int> run(
        Project &project,
        const std::vector<std::string> &args) override;
    /**
     * @copydoc Command::complete()
     */
    virtual boost::optional<int> complete(
        Project &project,
        const std::vector<std::string> &args) override;

private:
    /**
     * @brief Performs real processing of args on the configuration.
     *
     * @param config Configuration to work with.
     * @param args What to do with it.
     *
     * @returns Error code.
     */
    int run(Config &config, const std::vector<std::string> &args);
    /**
     * @brief Lists configuration keys along with their values.
     *
     * Builtin keys are not listed.
     *
     * @param config Configuration to read keys from.
     *
     * @returns Error code.
     */
    int printAllValues(Config &config);
    /**
     * @brief Prints single key-value pair.
     *
     * @param config Configuration.
     * @param key Name of key to print.
     */
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
ConfigCmd::run(Scribe &scribe, const std::vector<std::string> &args)
{
    po::options_description visible("config sub-command options");
    visible.add_options()
        ("help", "display help message")
        ("global", "use global configuration");

    po::options_description hidden;
    hidden.add_options()
        ("expressions", po::value<std::vector<std::string>>(),
            "assigns/displays values");

    po::options_description all;
    all.add(visible).add(hidden);

    po::positional_options_description p;
    p.add("expressions", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(args).options(all).positional(p).run(),
              vm);
    po::notify(vm);

    if (vm.count("help")) {
        out() << visible;
        return EXIT_SUCCESS;
    }

    if (vm.count("global")) {
        Config &config = scribe.getConfig();
        if (vm.count("expressions")) {
            return run(config,
                       vm["expressions"].as<std::vector<std::string>>());
        }
        return run(config, {});
    }
    return {};
}

boost::optional<int>
ConfigCmd::run(Project &project, const std::vector<std::string> &args)
{
    return run(project.getConfig(), args);
}

int
ConfigCmd::run(Config &config, const std::vector<std::string> &args)
{
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
            out() << "Wrong key name: '" << key << "'\n";
            continue;
        }

        if (set) {
            config.set(key, value);
        } else {
            printKey(config, key);
        }
    }

    return EXIT_SUCCESS;
}

boost::optional<int>
ConfigCmd::complete(Project &project, const std::vector<std::string> &args)
{
    Config &config = project.getConfig();

    if (!args.empty() && args.back().find('=') != std::string::npos) {
        std::string last = args.back();
        last.pop_back();
        out() << '\'' << config.get(last, "") << "'\n";
        return EXIT_SUCCESS;
    }

    std::unordered_set<std::string> keys;
    {
        std::vector<std::string> allKeys = config.list();
        keys.insert(allKeys.cbegin(), allKeys.cend());
    }

    for (const std::string &arg : args) {
        const std::string::size_type pos = arg.find('=');
        if (pos != 0U) {
            keys.erase(arg.substr(0U, pos));
        }
    }

    for (const std::string &key : keys) {
        out() << key << '\n';
    }

    return EXIT_SUCCESS;
}

int
ConfigCmd::printAllValues(Config &config)
{
    for (const std::string &key : config.list()) {
        printKey(config, key);
    }
    return EXIT_SUCCESS;
}

void
ConfigCmd::printKey(Config &config, const std::string &key)
{
    const std::string val = config.get(key, "<not set>");
    if (!val.empty()) {
        out() << decor::bold << key << decor::def << " = " << val << '\n';
    }
}
