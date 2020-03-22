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

#ifndef DIT__INVOCATION_HPP__
#define DIT__INVOCATION_HPP__

#include <functional>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief Implements command-line parsing and expansion.
 */
class Invocation
{
public:
    /**
     * @brief Maps alias name to its right-hand side.
     *
     * @param name Name of an alias.
     *
     * @returns Right-hand side of the alias or empty string on unknown alias.
     */
    using aliasResolverFunc =
        std::function<std::string(const std::string &name)>;

    /**
     * @brief Performs initialization from command line.
     *
     * @param args List of all the arguments (*without* application name).
     */
    void setCmdLine(std::vector<std::string> args);
    /**
     * @brief Sets project name to use by default (if none supplied).
     *
     * @param prjName Name of default project.
     */
    void setDefPrjName(std::string prjName);
    /**
     * @brief Sets default command-line to use by default (on empty input).
     *
     * The @p cmdLine is broken close to shell, but without expansions.
     *
     * @param cmdLine Default command-line.
     */
    void setDefCmdLine(std::string cmdLine);
    /**
     * @brief Sets function that resolves RHS of an alias by its name.
     *
     * @param resolver The function that maps alias name -> RHS.
     */
    void setAliasResolver(aliasResolverFunc resolver);

    /**
     * @brief Parses the command-line.
     *
     * The object should be configured via setters before invoking the method:
     *  * setCmdLine()
     *  * setDefPrjName()
     *  * setDefCmdLine()
     *  * setAliasResolver()
     *
     * @param completion Whether this is for command-line completion.
     *
     * @throws std::bad_function_call on uninitialized alias resolver.
     */
    void parse(bool completion = false);

    /**
     * @brief Whether usage information should be displayed.
     *
     * @returns @c true if so, @c false otherwise.
     */
    bool shouldPrintHelp() const;
    /**
     * @brief Returns usage information related to command-line options.
     *
     * @returns The information.
     */
    std::string getHelp() const;
    /**
     * @brief Retrieves name of command-line options.
     *
     * To be used for completion.
     *
     * @returns List of the names.
     */
    std::vector<std::string> getOpts() const;
    /**
     * @brief Whether version information should be displayed.
     *
     * @returns @c true if so, @c false otherwise.
     */
    bool shouldPrintVersion() const;
    /**
     * @brief Retrieves name of the project to act on.
     *
     * @returns The name.
     */
    std::string getPrjName() const;
    /**
     * @brief Retrieves name of the command to execute.
     *
     * @returns The name.
     */
    std::string getCmdName() const;
    /**
     * @brief Retrieves unexpanded composition of the commands.
     *
     * @returns The composition.
     */
    std::string getComposition() const;
    /**
     * @brief Retrieves arguments of the command (@c getCmdName()).
     *
     * @returns Arguments of command.
     */
    std::vector<std::string> getCmdArgs() const;
    /**
     * @brief Retrieves configuration override.
     *
     * @returns Key-value pairs to use for the override.
     */
    std::vector<std::pair<std::string, std::string>> getConfs() const;

private:
    /**
     * @brief Parses command-line options (like @c --help).
     *
     * Initializes @c cmdLine for further processing.
     *
     * @param completion Whether this is for command-line completion.
     */
    void processOptions(bool completion);

private:
    /**
     * @brief Name of the project to operate on.
     */
    std::string prjName;
    /**
     * @brief Key-value pairs of configuration overrides.
     */
    std::vector<std::pair<std::string, std::string>> confs;
    /**
     * @brief Actual name of the command to be executed.
     */
    std::string cmdName;
    /**
     * @brief Unexpanded composition of command names.
     */
    std::string composition;
    /**
     * @brief Arguments of the application.
     */
    std::vector<std::string> cmdLine;
    /**
     * @brief Project name to use if project is not specified.
     */
    std::string defPrjName;
    /**
     * @brief Command line to use if no arguments are supplied.
     */
    std::string defCmdLine;
    /**
     * @brief Function that resolves alias name into its right-hand side.
     */
    aliasResolverFunc aliasResolver;
    /**
     * @brief Whether usage information should be displayed.
     */
    bool help = false;
    /**
     * @brief Usage information related to command-line options.
     */
    std::string helpMsg;
    /**
     * @brief Retrieves name of command-line options.
     */
    std::vector<std::string> optNames;
    /**
     * @brief Whether version information should be displayed.
     */
    bool version = false;
};

#endif // DIT__INVOCATION_HPP__
