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

#include "decoration.hpp"

#include <ostream>

#include "integration.hpp"

namespace {

/**
 * @brief Actual implementation of color attributes.
 */
class Colors
{
public:
    /**
     * @brief Constructs the class checking whether stdout is a terminal.
     */
    void disable() { isAscii = false; }

    const char * bold () { return isAscii ? "\033[1m" : ""; }
    const char * inv  () { return isAscii ? "\033[7m" : ""; }
    const char * def  () { return isAscii ? "\033[1m\033[0m" : ""; }

    const char * black_fg   () { return isAscii ? "\033[30m" : ""; }
    const char * red_fg     () { return isAscii ? "\033[31m" : ""; }
    const char * green_fg   () { return isAscii ? "\033[32m" : ""; }
    const char * yellow_fg  () { return isAscii ? "\033[33m" : ""; }
    const char * blue_fg    () { return isAscii ? "\033[34m" : ""; }
    const char * magenta_fg () { return isAscii ? "\033[35m" : ""; }
    const char * cyan_fg    () { return isAscii ? "\033[36m" : ""; }
    const char * white_fg   () { return isAscii ? "\033[37m" : ""; }

    const char * black_bg   () { return isAscii ? "\033[40m" : ""; }
    const char * red_bg     () { return isAscii ? "\033[41m" : ""; }
    const char * green_bg   () { return isAscii ? "\033[42m" : ""; }
    const char * yellow_bg  () { return isAscii ? "\033[43m" : ""; }
    const char * blue_bg    () { return isAscii ? "\033[44m" : ""; }
    const char * magenta_bg () { return isAscii ? "\033[45m" : ""; }
    const char * cyan_bg    () { return isAscii ? "\033[46m" : ""; }
    const char * white_bg   () { return isAscii ? "\033[47m" : ""; }

private:
    /**
     * @brief Whether outputting of ASCII escape sequences is enabled.
     */
    bool isAscii = isOutputToTerminal();
} C;

}

// Shorten type name to fit into 80 columns limit.
using ostr = std::ostream;

using namespace decor;

const Decoration
    decor::none,
    decor::bold       ([](ostr &os) -> ostr & { return os << C.bold();       }),
    decor::inv        ([](ostr &os) -> ostr & { return os << C.inv();        }),
    decor::def        ([](ostr &os) -> ostr & { return os << C.def();        }),

    decor::black_fg   ([](ostr &os) -> ostr & { return os << C.black_fg();   }),
    decor::red_fg     ([](ostr &os) -> ostr & { return os << C.red_fg();     }),
    decor::green_fg   ([](ostr &os) -> ostr & { return os << C.green_fg();   }),
    decor::yellow_fg  ([](ostr &os) -> ostr & { return os << C.yellow_fg();  }),
    decor::blue_fg    ([](ostr &os) -> ostr & { return os << C.blue_fg();    }),
    decor::magenta_fg ([](ostr &os) -> ostr & { return os << C.magenta_fg(); }),
    decor::cyan_fg    ([](ostr &os) -> ostr & { return os << C.cyan_fg();    }),
    decor::white_fg   ([](ostr &os) -> ostr & { return os << C.white_fg();   }),

    decor::black_bg   ([](ostr &os) -> ostr & { return os << C.black_bg();   }),
    decor::red_bg     ([](ostr &os) -> ostr & { return os << C.red_bg();     }),
    decor::green_bg   ([](ostr &os) -> ostr & { return os << C.green_bg();   }),
    decor::yellow_bg  ([](ostr &os) -> ostr & { return os << C.yellow_bg();  }),
    decor::blue_bg    ([](ostr &os) -> ostr & { return os << C.blue_bg();    }),
    decor::magenta_bg ([](ostr &os) -> ostr & { return os << C.magenta_bg(); }),
    decor::cyan_bg    ([](ostr &os) -> ostr & { return os << C.cyan_bg();    }),
    decor::white_bg   ([](ostr &os) -> ostr & { return os << C.white_bg();   });

Decoration::Decoration(const Decoration &rhs)
    : decorator(rhs.decorator),
      lhs(rhs.lhs == nullptr ? nullptr : new Decoration(*rhs.lhs)),
      rhs(rhs.rhs == nullptr ? nullptr : new Decoration(*rhs.rhs))
{
}

Decoration::Decoration(decorFunc decorator) : decorator(decorator)
{
}

Decoration::Decoration(const Decoration &lhs, const Decoration &rhs)
    : lhs(new Decoration(lhs)),
      rhs(new Decoration(rhs))
{
}

Decoration &
Decoration::operator=(const Decoration &rhs)
{
    Decoration copy(rhs);
    std::swap(copy.decorator, this->decorator);
    std::swap(copy.lhs, this->lhs);
    std::swap(copy.rhs, this->rhs);
    return *this;
}

std::ostream &
Decoration::decorate(std::ostream &os) const
{
    if (decorator != nullptr) {
        // Reset and preserve width field, so printing escape sequence doesn't
        // mess up formatting.
        const auto width = os.width({});
        os << decorator;
        static_cast<void>(os.width(width));
        return os;
    }
    if (lhs != nullptr && rhs != nullptr) {
        return os << *lhs << *rhs;
    }
    return os;
}

std::ostream &
ScopedDecoration::decorate(std::ostream &os) const
{
    os << decoration;
    for (const auto app : apps) {
        app(os);
    }
    os << def;
    return os;
}

void
decor::disableDecorations(pk<Tests>)
{
    C.disable();
}
