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

#include "integration.hpp"

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include <cstdio>

#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/algorithm/string/trim.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/optional.hpp>
#include <boost/scope_exit.hpp>

#include "utils/fs.hpp"
#include "utils/memory.hpp"

namespace io = boost::iostreams;

static std::pair<unsigned int, unsigned int> getTerminalSize();
static void writeBufferFile(const std::string &path, const std::string &key,
                            const std::string &current);
static bool editBufferFile(const std::string &path);
static std::string readEditedValue(const std::string &path);

/**
 * @brief Base for hidden internals of RedirectToPager class.
 */
class RedirectToPager::Impl
{
public:
    //! Virtual destructor.
    virtual ~Impl() = default;
};

/**
 * @brief Redirects standard output into a pager.
 *
 * The redirection happens only if number of lines exceeds screen height,
 * otherwise lines are just dumped onto the screen as is.
 */
class PagerRedirect : public RedirectToPager::Impl
{
public:
    /**
     * @brief Custom stream buffer that spawns pager for large outputs only.
     *
     * Collect up to terminal height lines.  If buffer is closed with this limit
     * not reached, it prints lines on std::cout.  If we hit the limit in the
     * process of output, it opens a pager and feeds it all collected output and
     * everything that comes next.
     */
    class ScreenPageBuffer
    {
    public:
        //! Type of character used by this buffer.
        using char_type = char;
        //! Category of functionality provided by this buffer implementation.
        using category = boost::iostreams::sink_tag;

    public:
        /**
         * @brief Constructs the buffer.
         *
         * @param pagerCmd Command to invoke a pager.
         * @param screenHeight Height of terminal in lines.
         * @param out Storage for output stream buffer backed up by a file.
         */
        ScreenPageBuffer(const std::string &pagerCmd,
                         unsigned int screenHeight,
                         io::stream_buffer<io::file_descriptor_sink> *out);
        /**
         * @brief Dumps output onto the screen or waits for pager to finish.
         */
        ~ScreenPageBuffer();

    public:
        /**
         * @brief Writes @p n characters from @p s.
         *
         * @param s Character buffer.
         * @param n Size of the buffer.
         *
         * @returns Number of successfully written characters.
         */
        std::streamsize write(const char s[], std::streamsize n);

    private:
        /**
         * @brief Writes single character.
         *
         * @param c Character to write.
         *
         * @returns @c true on success, @c false otherwise.
         */
        bool put(char c);
        /**
         * @brief Opens pager for output.
         */
        void openPager();

    private:
        //! Whether redirection into pager is enabled.
        bool redirectToPager = false;
        //! Number of output lines collected so far.
        unsigned int nLines = 0U;
        //! Command to invoke a pager.
        std::string pagerCmd;
        //! Height of terminal in lines.
        unsigned int screenHeight;
        //! Output collected so far.
        std::string buffer;
        //! Process id of a pager.
        pid_t pid;

        /**
         * @brief Pointer to buffer stored in RedirectToPager.
         *
         * This is not by value, because ScreenPageBuffer needs to be copyable.
         */
        io::stream_buffer<io::file_descriptor_sink> *out;
    };

public:
    /**
     * @brief Replaces buffer of @c std::cout with ScreenPageBuffer.
     *
     * @param pagerCmd Command to invoke a pager.
     */
    PagerRedirect(const std::string &pagerCmd)
        : screenPageBuffer(pagerCmd, getTerminalSize().second, &out)
    {
        rdbuf = std::cout.rdbuf(&screenPageBuffer);
    }

    /**
     * @brief Restores original buffer of @c std::cout.
     */
    ~PagerRedirect()
    {
        // Flush the stream to make sure that we put all contents we want
        // through the custom stream buffer.
        std::cout.flush();

        std::cout.rdbuf(rdbuf);
    }

private:
    //! This is stored for ScreenPageBuffer class.
    io::stream_buffer<io::file_descriptor_sink> out;
    //! Custom buffer implementation.
    io::stream_buffer<ScreenPageBuffer> screenPageBuffer;
    //! Original buffer of @c std::cout.
    std::streambuf *rdbuf;
};

using ScreenPageBuffer = PagerRedirect::ScreenPageBuffer;

ScreenPageBuffer::ScreenPageBuffer(const std::string &pagerCmd,
                                   unsigned int screenHeight,
                               io::stream_buffer<io::file_descriptor_sink> *out)
    : pagerCmd(pagerCmd), screenHeight(screenHeight - 1U), out(out)
{
}

ScreenPageBuffer::~ScreenPageBuffer()
{
    if (redirectToPager) {
        out->close();
        int wstatus;
        waitpid(pid, &wstatus, 0);
    } else {
        std::cout << buffer;
    }
}

std::streamsize
ScreenPageBuffer::write(const char s[], std::streamsize n)
{
    for (std::streamsize i = 0U; i < n; ++i) {
        if (!put(s[i])) {
            return i;
        }
    }
    return n;
}

bool
ScreenPageBuffer::put(char c)
{
    if (redirectToPager) {
        return boost::iostreams::put(*out, c);
    }

    if (c == '\n') {
        ++nLines;
    }

    if (nLines > screenHeight) {
        openPager();
        redirectToPager = true;
        for (char c : buffer) {
            if (!boost::iostreams::put(*out, c)) {
                return false;
            }
        }
        return boost::iostreams::put(*out, c);
    }

    buffer.push_back(c);
    return true;
}

void
ScreenPageBuffer::openPager()
{
    int pipePair[2];
    if (pipe(pipePair) != 0) {
        throw std::runtime_error("Failed to create a pipe");
    }
    BOOST_SCOPE_EXIT_ALL(pipePair) { close(pipePair[0]); };

    pid = fork();
    if (pid == -1) {
        close(pipePair[1]);
        throw std::runtime_error("Fork has failed");
    }
    if (pid == 0) {
        close(pipePair[1]);
        if (dup2(pipePair[0], STDIN_FILENO) == -1) {
            _Exit(EXIT_FAILURE);
        }
        close(pipePair[0]);
        execlp("/bin/sh", "sh", "-c", pagerCmd.c_str(),
               static_cast<char *>(nullptr));
        _Exit(127);
    }

    out->open(io::file_descriptor_sink(pipePair[1],
                                       boost::iostreams::close_handle));
}

RedirectToPager::RedirectToPager(const std::string &pagerCmd)
    : impl(isOutputToTerminal() ? make_unique<PagerRedirect>(pagerCmd)
                                : nullptr)
{
}

RedirectToPager::~RedirectToPager() = default;

/**
 * @brief Retrieves terminal width and height in characters.
 *
 * @returns Pair of actual terminal width and height, or maximum possible values
 *          of the type.
 */
static std::pair<unsigned int, unsigned int>
getTerminalSize()
{
    winsize ws;
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) != 0) {
        return {
            std::numeric_limits<unsigned int>::max(),
            std::numeric_limits<unsigned int>::max()
        };
    }

    return { ws.ws_col, ws.ws_row };
}

boost::optional<std::string>
editValue(const std::string &key, const std::string &current)
{
    // Generate name of a temporary file and write the file.
    TempFile tmpFile("buf");
    writeBufferFile(tmpFile, key, current);

    if (!editBufferFile(tmpFile)) {
        return {};
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
    return value;
}

bool
isOutputToTerminal()
{
    return isatty(fileno(stdout));
}

unsigned int
getTerminalWidth()
{
    return getTerminalSize().first;
}
