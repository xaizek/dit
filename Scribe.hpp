#ifndef SCRIBE__SCRIBE_HPP__
#define SCRIBE__SCRIBE_HPP__

#include <string>
#include <vector>

/**
 * @brief Application implementation.
 */
class Scribe
{
public:
    /**
     * @brief Constructs main application class.
     *
     * @param argc Number of application arguments.
     * @param argv Application arguments.
     *
     * @throws std::runtime_error on broken argument list.
     */
    Scribe(int argc, const char *const argv[]);

public:
    /**
     * @brief Entry point of the application.
     *
     * @returns Exit status of the applicatio (to be returned by @c main()).
     */
    int run();

private:
    /**
     * @brief Name used to run the application.
     */
    std::string appName;
    /**
     * @brief Command to execute.
     */
    std::string cmd;
    /**
     * @brief Arguments for the command specified by @c cmd.
     */
    std::vector<std::string> args;
};

#endif // SCRIBE__SCRIBE_HPP__
