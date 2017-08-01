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

#ifndef DIT__DECORATION_HPP__
#define DIT__DECORATION_HPP__

#include <functional>
#include <memory>
#include <iosfwd>
#include <type_traits>
#include <vector>

#include "utils/Passkey.hpp"

class Tests;

/**
 * @brief Terminal control manipulators for output streams.
 *
 * Usage example:
 * @code
 * std::cout << decor::bold << "This is bold text." << decor::def;
 * @endcode
 *
 * Equivalent example with scope:
 * @code
 * std::cout << (decor::bold << "This is bold text.");
 * @endcode
 */
namespace decor {

/**
 * @brief Class describing single decoration or a combination of them.
 */
class Decoration
{
    using decorFunc = std::ostream & (*)(std::ostream &os);

public:
    /**
     * @brief Constructs possibly empty decoration.
     *
     * @param decorator Decorating function.
     */
    explicit Decoration(decorFunc decorator = nullptr);
    /**
     * @brief Constructs a (deep) copy of a decoration.
     *
     * @param rhs Original object to be copied.
     */
    Decoration(const Decoration &rhs);
    /**
     * @brief Constructs a decoration that is a combination of others.
     *
     * @param lhs First decoration to combine.
     * @param rhs Second decoration to combine.
     */
    Decoration(const Decoration &lhs, const Decoration &rhs);

public:
    /**
     * @brief Assigns one decoration to another.
     *
     * @param rhs Assignment operand.
     *
     * @returns @c *this.
     */
    Decoration & operator=(const Decoration &rhs);

    /**
     * @brief Actually performs the decoration of a stream.
     *
     * @param os Stream to decorate.
     *
     * @returns @p os.
     */
    std::ostream & decorate(std::ostream &os) const;

private:
    /**
     * @brief Decoration function (can be nullptr).
     */
    decorFunc decorator = nullptr;
    /**
     * @brief One of two decorations that compose this object.
     */
    std::unique_ptr<Decoration> lhs;
    /**
     * @brief Second decoration that composes this object.
     */
    std::unique_ptr<Decoration> rhs;
};

/**
 * @brief Combines two decorations to create a new one.
 *
 * @param lhs First decoration to combine.
 * @param rhs Second decoration to combine.
 *
 * @returns The combination.
 */
inline Decoration
operator+(const Decoration &lhs, const Decoration &rhs)
{
    return Decoration(lhs, rhs);
}

/**
 * @brief A thunk for decoration use with @c std::ostream.
 *
 * @param os Stream to output to.
 * @param d Decoration to apply.
 *
 * @returns @p os.
 */
inline std::ostream &
operator<<(std::ostream &os, const Decoration &d)
{
    return d.decorate(os);
}

/**
 * @brief A scoped decoration container.
 *
 * Collects data that should be output to a stream and later replays it.
 */
class ScopedDecoration
{
public:
    /**
     * @brief Constructs an object from decoration and stream action.
     *
     * @param decoration Decoration of the scope.
     * @param app First application in the scope.
     */
    ScopedDecoration(const Decoration &decoration,
                     std::function<void(std::ostream&)> app)
        : decoration(decoration)
    {
        apps.push_back(app);
    }
    /**
     * @brief Constructs an object from scope and additional stream action.
     *
     * @param scoped Pre-existing scoped decoration which is being extended.
     * @param app Additional application in the scope.
     */
    ScopedDecoration(const ScopedDecoration &scoped,
                     std::function<void(std::ostream&)> app)
        : decoration(scoped.decoration), apps(scoped.apps)
    {
        apps.push_back(app);
    }

public:
    /**
     * @brief Actually performs the decoration of a stream.
     *
     * @param os Stream to decorate.
     *
     * @returns @p os.
     */
    std::ostream & decorate(std::ostream &os) const;

private:
    /**
     * @brief Decoration object of this scope.
     */
    const Decoration &decoration;
    /**
     * @brief Stream actions in this scope.
     */
    std::vector<std::function<void(std::ostream&)>> apps;
};

/**
 * @brief Constructs scoped decoration.
 *
 * @tparam T Type of stream action.
 * @param d Decoration of the scope.
 * @param v Stream action.
 *
 * @returns Scoped decoration constructed.
 */
template <typename T>
typename std::enable_if<std::is_function<T>::value, ScopedDecoration>::type
operator<<(const Decoration &d, const T &v)
{
    const T *val = v;
    return ScopedDecoration(d, [val](std::ostream &os) { os << val; });
}

/**
 * @brief Constructs scoped decoration.
 *
 * @tparam T Type of stream data.
 * @param d Decoration of the scope.
 * @param v Stream data.
 *
 * @returns Scoped decoration constructed.
 */
template <typename T>
typename std::enable_if<!std::is_function<T>::value, ScopedDecoration>::type
operator<<(const Decoration &d, const T &val)
{
    return ScopedDecoration(d, [val](std::ostream &os) { os << val; });
}

/**
 * @brief Appends to scoped decoration.
 *
 * @tparam T Type of stream data.
 * @param sd Scoped decoration.
 * @param v Stream data.
 *
 * @returns Scoped decoration constructed.
 */
template <typename T>
ScopedDecoration
operator<<(const ScopedDecoration &sd, const T &val)
{
    return ScopedDecoration(sd, [val](std::ostream &os) { os << val; });
}

/**
 * @brief A thunk for scoped decoration use with @c std::ostream.
 *
 * @param os Stream to output to.
 * @param sd Scoped decoration to apply.
 *
 * @returns @p os.
 */
inline std::ostream &
operator<<(std::ostream &os, const ScopedDecoration &sd)
{
    return sd.decorate(os);
}

/**
 * @{
 * @name Generic attributes
 */

/**
 * @brief Convenient attribute that does nothing.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
extern const Decoration none;
/**
 * @brief Enables bold attribute.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
extern const Decoration bold;
/**
 * @brief Enables color inversion attribute.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
extern const Decoration inv;
/**
 * @brief Restores default attribute of the stream.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
extern const Decoration def;

/**
 * @}
 *
 * @{
 * @name Foreground colors
 */

/**
 * @brief Picks black as foreground color.
 */
extern const Decoration black_fg;
/**
 * @brief Picks red as foreground color.
 */
extern const Decoration red_fg;
/**
 * @brief Picks green as foreground color.
 */
extern const Decoration green_fg;
/**
 * @brief Picks yellow as foreground color.
 */
extern const Decoration yellow_fg;
/**
 * @brief Picks blue as foreground color.
 */
extern const Decoration blue_fg;
/**
 * @brief Picks magenta as foreground color.
 */
extern const Decoration magenta_fg;
/**
 * @brief Picks cyan as foreground color.
 */
extern const Decoration cyan_fg;
/**
 * @brief Picks white as foreground color.
 */
extern const Decoration white_fg;

/**
 * @}
 *
 * @{
 * @name Background colors
 */

/**
 * @brief Picks black as background color.
 */
extern const Decoration black_bg;
/**
 * @brief Picks red as background color.
 */
extern const Decoration red_bg;
/**
 * @brief Picks green as background color.
 */
extern const Decoration green_bg;
/**
 * @brief Picks yellow as background color.
 */
extern const Decoration yellow_bg;
/**
 * @brief Picks blue as background color.
 */
extern const Decoration blue_bg;
/**
 * @brief Picks magenta as background color.
 */
extern const Decoration magenta_bg;
/**
 * @brief Picks cyan as background color.
 */
extern const Decoration cyan_bg;
/**
 * @brief Picks white as background color.
 */
extern const Decoration white_bg;

/**
 * @}
 *
 * @{
 * @name Control
 */

/**
 * @brief Forces disabling of decorations.
 */
void disableDecorations(pk<Tests>);

/**
 * @}
 */

}

#endif // DIT__DECORATION_HPP__
