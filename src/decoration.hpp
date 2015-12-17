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

#include <iosfwd>

#include "utils/Passkey.hpp"

class Tests;

/**
 * @brief Terminal control manipulators for output streams.
 *
 * Usage example:
 * @code
 * std::cout << decor::bold << "This is bold text." << decor::def;
 * @endcode
 */
namespace decor {

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
std::ostream & none(std::ostream &os);
/**
 * @brief Enables bold attribute.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & bold(std::ostream &os);
/**
 * @brief Enables color inversion attribute.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & inv(std::ostream &os);
/**
 * @brief Restores default attribute of the stream.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & def(std::ostream &os);

/**
 * @}
 *
 * @{
 * @name Foreground colors
 */

/**
 * @brief Picks black as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & black_fg(std::ostream &os);
/**
 * @brief Picks red as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & red_fg(std::ostream &os);
/**
 * @brief Picks green as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & green_fg(std::ostream &os);
/**
 * @brief Picks yellow as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & yellow_fg(std::ostream &os);
/**
 * @brief Picks blue as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & blue_fg(std::ostream &os);
/**
 * @brief Picks magenta as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & magenta_fg(std::ostream &os);
/**
 * @brief Picks cyan as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & cyan_fg(std::ostream &os);
/**
 * @brief Picks white as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & white_fg(std::ostream &os);

/**
 * @}
 *
 * @{
 * @name Background colors
 */

/**
 * @brief Picks black as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & black_bg(std::ostream &os);
/**
 * @brief Picks red as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & red_bg(std::ostream &os);
/**
 * @brief Picks green as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & green_bg(std::ostream &os);
/**
 * @brief Picks yellow as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & yellow_bg(std::ostream &os);
/**
 * @brief Picks blue as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & blue_bg(std::ostream &os);
/**
 * @brief Picks magenta as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & magenta_bg(std::ostream &os);
/**
 * @brief Picks cyan as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & cyan_bg(std::ostream &os);
/**
 * @brief Picks white as foreground color.
 *
 * @param os Stream to operate on.
 *
 * @returns @p os
 */
std::ostream & white_bg(std::ostream &os);

/**
 * @}
 *
 * @{
 * @name Control
 */

/**
 * @brief Controls state of decorations.
 *
 * @param enabled Whether decorations should be enabled or do nothing.
 */
void setEnabled(bool enabled, pk<Tests>);

/**
 * @}
 */

}

#endif // DIT__DECORATION_HPP__
