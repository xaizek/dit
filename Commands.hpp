#ifndef SCRIBE__COMMANDS_HPP__
#define SCRIBE__COMMANDS_HPP__

#include <map>
#include <memory>
#include <string>
#include <vector>

class Command;

/**
 * @brief Keeps track of all sub-commands.
 */
class Commands
{
private:
    /**
     * @brief This class is not ment to be instantiated from the outside.
     */
    Commands() = default;

public:
    /**
     * @brief Registers command.
     * @see REGISTER_COMMAND
     *
     * @param cmd An instance of the command.
     *
     * @throws std::runtime_error On duplicated command name.
     */
    static void add(std::unique_ptr<Command> cmd);
    /**
     * @brief Retrieves command by its name.
     *
     * @param name Name of command to look up for.
     *
     * @returns The command or @c nullptr if command not found.
     */
    static Command * get(const std::string &name);
    /**
     * @brief Lists all available commands.
     *
     * @returns Vector of all commands.
     */
    static std::vector<Command *> list();

private:
    /**
     * @brief Provides access to Commands instance.
     *
     * @returns The instance.
     */
    static Commands & getThis();

private:
    /**
     * @brief All available commands, registered via @c add().
     */
    std::map<std::string, std::unique_ptr<Command>> commands;
};

/**
 * @brief Helper macro that auto-registers Command-class in Commands-list.
 *
 * @param className Name of the class derived from @c Command.
 */
#define REGISTER_COMMAND(className) \
    static int \
    doReg ## className() \
    { \
        Commands::add(std::unique_ptr<Command>(new className())); \
        return 0; \
    } \
    const int reg ## className = doReg ## className();

#endif // SCRIBE__COMMANDS_HPP__
