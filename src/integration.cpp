// Copyright (C) 2015 xaizek <xaizek@openmailbox.org>
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

#include "integration.hpp"

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <fstream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/algorithm/string/trim.hpp>
#include <boost/optional.hpp>

#include "utils/fs.hpp"

static void writeBufferFile(const std::string &path, const std::string &key,
                            const std::string &current);
static bool editBufferFile(const std::string &path);
static std::string readEditedValue(const std::string &path);

boost::optional<std::string>
editValue(const std::string &key, const std::string &value,
          const std::string &current)
{
    if (value != "-") {
        return {};
    }

    // Create name for temporary file name and the file itself.
    TempFile tmpFile("buf");
    writeBufferFile(tmpFile, key, current);

    if (!editBufferFile(tmpFile)) {
        return current;
    }

    return readEditedValue(tmpFile);
}

/**
 * @brief Writes buffer file with which user will be presented.
 *
 * @param path Path to temporary file.
 * @param key Key (name of the value).
 * @param current Current value to be edited.
 */
static void
writeBufferFile(const std::string &path, const std::string &key,
                const std::string &current)
{
    std::ofstream ofs(path);
    if (!ofs) {
        throw std::runtime_error("Failed to open temporary file: " + path);
    }

    // TODO: add project name and description of action to the file.
    // TODO: maybe additional text should be placed at the bottom.

    // Write current value and instructions to the file.
    ofs << "# Edit value for the \"" << key << "\" below.  First (this) line "
           "will be ignored.\n" << (current.empty() ? "\n" : current);
}

/**
 * @brief Invokes editor to edit the file.
 *
 * @param path Path to the file that should be opened.
 *
 * @returns @c true on success and @c false on cancellation/error.
 */
static bool
editBufferFile(const std::string &path)
{
    // TODO: maybe check for empty file and treat it as an error.

    const char *editorVar = std::getenv("EDITOR");
    std::string editor = (editorVar == NULL) ? "vim" : editorVar;

    int status = std::system((editor + " " + path).c_str());
    return (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS);
}

/**
 * @brief Reads new value from a file.
 *
 * @param path Path to temporary file.
 *
 * @returns The value.
 */
static std::string
readEditedValue(const std::string &path)
{
    std::ifstream ifs(path);
    if (!ifs) {
        throw std::runtime_error("Failed to open temporary file: " + path);
    }

    // Discard first line.
    ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Read the rest of the file.
    std::string value {
        std::istreambuf_iterator<char>(ifs),
        std::istreambuf_iterator<char>()
    };

    boost::trim_if(value, [](char c) { return std::isspace(c) || c == '\n'; } );
    return std::move(value);
}

unsigned int
getTerminalWidth()
{
    winsize ws;
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) != 0) {
        return std::numeric_limits<unsigned int>::max();
    }

    return ws.ws_col;
}
