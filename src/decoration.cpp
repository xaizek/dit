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

#include "decoration.hpp"

#include <unistd.h>

#include <cstdio>

#include <ostream>

namespace {

/**
 * @brief Whether decorations are enabled.
 */
static bool isEnabled = true;

/**
 * @brief Actual implementation of color attributes.
 */
class Colors
{
public:
    /**
     * @brief Constructs the class checking whether stdout is a terminal.
     */
    Colors() : isAscii(isatty(fileno(stdout)))
    {
    }

public:
    const char * bold () { return active() ? "\033[1m" : ""; }
    const char * inv  () { return active() ? "\033[7m" : ""; }
    const char * def  () { return active() ? "\033[1m\033[0m" : ""; }

    const char * black_fg   () { return active() ? "\033[30m" : ""; }
    const char * red_fg     () { return active() ? "\033[31m" : ""; }
    const char * green_fg   () { return active() ? "\033[32m" : ""; }
    const char * yellow_fg  () { return active() ? "\033[33m" : ""; }
    const char * blue_fg    () { return active() ? "\033[34m" : ""; }
    const char * magenta_fg () { return active() ? "\033[35m" : ""; }
    const char * cyan_fg    () { return active() ? "\033[36m" : ""; }
    const char * white_fg   () { return active() ? "\033[37m" : ""; }

    const char * black_bg   () { return active() ? "\033[40m" : ""; }
    const char * red_bg     () { return active() ? "\033[41m" : ""; }
    const char * green_bg   () { return active() ? "\033[42m" : ""; }
    const char * yellow_bg  () { return active() ? "\033[43m" : ""; }
    const char * blue_bg    () { return active() ? "\033[44m" : ""; }
    const char * magenta_bg () { return active() ? "\033[45m" : ""; }
    const char * cyan_bg    () { return active() ? "\033[46m" : ""; }
    const char * white_bg   () { return active() ? "\033[47m" : ""; }

private:
    bool active() const
    {
        return isAscii && isEnabled;
    }

private:
    /**
     * @brief Whether outputting of ASCII escape sequences is enabled.
     */
    const bool isAscii;
} C;

}

// Shorten type name to fit into 80 columns limit.
using ostr = std::ostream;

ostr & decor::none (ostr &os) { return os; }
ostr & decor::bold (ostr &os) { return os << C.bold(); }
ostr & decor::inv  (ostr &os) { return os << C.inv(); }
ostr & decor::def  (ostr &os) { return os << C.def(); }

ostr & decor::black_fg   (ostr &os) { return os << C.black_fg(); }
ostr & decor::red_fg     (ostr &os) { return os << C.red_fg(); }
ostr & decor::green_fg   (ostr &os) { return os << C.green_fg(); }
ostr & decor::yellow_fg  (ostr &os) { return os << C.yellow_fg(); }
ostr & decor::blue_fg    (ostr &os) { return os << C.blue_fg(); }
ostr & decor::magenta_fg (ostr &os) { return os << C.magenta_fg(); }
ostr & decor::cyan_fg    (ostr &os) { return os << C.cyan_fg(); }
ostr & decor::white_fg   (ostr &os) { return os << C.white_fg(); }

ostr & decor::black_bg   (ostr &os) { return os << C.black_bg(); }
ostr & decor::red_bg     (ostr &os) { return os << C.red_bg(); }
ostr & decor::green_bg   (ostr &os) { return os << C.green_bg(); }
ostr & decor::yellow_bg  (ostr &os) { return os << C.yellow_bg(); }
ostr & decor::blue_bg    (ostr &os) { return os << C.blue_bg(); }
ostr & decor::magenta_bg (ostr &os) { return os << C.magenta_bg(); }
ostr & decor::cyan_bg    (ostr &os) { return os << C.cyan_bg(); }
ostr & decor::white_bg   (ostr &os) { return os << C.white_bg(); }

void
decor::setEnabled(bool enabled, pk<Tests>)
{
    isEnabled = enabled;
}
