// Copyright (C) 2015 xaizek <xaizek@posteo.net>
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

#include <cassert>
#include <cstdlib>

#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/program_options.hpp>

#include "utils/contains.hpp"
#include "utils/opts.hpp"
#include "utils/strings.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "Config.hpp"
#include "Dit.hpp"
#include "Project.hpp"
#include "integration.hpp"
#include "parsing.hpp"
#include "printing.hpp"

namespace po = boost::program_options;

/**
 * @brief Usage message for "config" command.
 */
const char *const USAGE =
R"(Usage: config [--help|-h] [--global|-g] [key[=val]...]

When invoked without arguments, settings are listed.

    key        --  displays configuration value
    key=value  --  sets configuration value
    key=-      --  spawns editor to edit configuration value)";

namespace {

/**
 * @brief Implementation of "config" command, which gets/sets configuration.
 */
class ConfigCmd : public AutoRegisteredCommand<ConfigCmd>
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
        Dit &dit,
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
        Dit &dit,
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

private:
    /**
     * @brief Saved pointer to global configuration.
     */
    Config *globalCfg;
    /**
     * @brief Options of the sub-command.
     */
    po::options_description opts;
};

}

ConfigCmd::ConfigCmd()
    : parent("config", "read/update configuration", USAGE),
      globalCfg(nullptr),
      opts("config sub-command options")
{
    opts.add_options()
        ("help,h", "display help message")
        ("global,g", "use global configuration");
}

boost::optional<int>
ConfigCmd::run(Dit &dit, const std::vector<std::string> &args)
{
    po::variables_map vm = parseOpts(args, opts);

    if (vm.count("help")) {
        out() << opts;
        return EXIT_SUCCESS;
    }

    if (vm.count("global")) {
        Config &config = dit.getConfig();
        if (vm.count("positional")) {
            return run(config,
                       vm["positional"].as<std::vector<std::string>>());
        }
        return run(config, {});
    }
    return {};
}

boost::optional<int>
ConfigCmd::run(Project &project, const std::vector<std::string> &args)
{
    return run(project.getConfig(false), args);
}

int
ConfigCmd::run(Config &config, const std::vector<std::string> &args)
{
    if (args.empty()) {
        return printAllValues(config);
    }

    for (const std::string &a : parsePairedArgs(args)) {
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

        if (!set) {
            printKey(config, key);
            continue;
        }

        if (value == "-") {
            boost::optional<std::string> v = editValue(key,
                                                       config.get(key, {}));
            if (!v) {
                err() << "Failed to prompt for value of key \""
                      << key << "\"\n";
                return EXIT_FAILURE;
            }
            value = std::move(*v);
        }
        config.set(key, value);
    }

    return EXIT_SUCCESS;
}

boost::optional<int>
ConfigCmd::complete(Dit &dit, const std::vector<std::string> &/*args*/)
{
    globalCfg = &dit.getConfig();
    return { };
}

boost::optional<int>
ConfigCmd::complete(Project &project, const std::vector<std::string> &args)
{
    assert(globalCfg != nullptr && "Global completion handler wasn't called.");

    po::variables_map vm = parseOpts(args, opts);
    Config &config = vm.count("global") ? *globalCfg : project.getConfig(false);

    if (!args.empty() && contains(args.back(), '=')) {
        std::string last = args.back();
        last.pop_back();
        out() << '\'' << config.get(last, "") << "'\n";
        return EXIT_SUCCESS;
    }

    std::set<std::string> keys;
    {
        std::vector<std::string> allKeys = config.list();
        keys.insert(allKeys.cbegin(), allKeys.cend());
    }

    // Remove elements already present on the command-line from completion list.
    for (const std::string &arg : args) {
        const std::string::size_type pos = arg.find('=');
        if (pos != 0U && pos != std::string::npos) {
            keys.erase(arg.substr(0U, pos));
            continue;
        }

        if (!arg.empty() && arg.back() == ':') {
            keys.erase(arg.substr(0U, arg.size() - 1U));
        }
    }

    using opt_t = boost::shared_ptr<po::option_description>;
    for (const opt_t &opt : opts.options()) {
        out() << "--" << opt->long_name() << '\n';
    }
    out() << "-h\n" << "-g\n";

    // Due to implicitly added space after completion match colon form is easier
    // to type.
    for (const std::string &key : keys) {
        out() << key << ":\n";
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
        out() << Key{key} << " = " << val << '\n';
    }
}
