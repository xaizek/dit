// Copyright (C) 2015 xaizek <xaizek@openmailbox.org>
//
// This file is part of scribe.
//
// scribe is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// scribe is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with scribe.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SCRIBE__UTILS__PASSKEY_HPP__
#define SCRIBE__UTILS__PASSKEY_HPP__

/**
 * @brief Passkey idiom helper class.
 *
 * @tparam T Type that has the permission to use the interface.
 */
template <typename T>
class Passkey
{
    friend T;

private:
    /**
     * @brief Only T should be able to instantiate this class.
     */
    Passkey() = default;

public:
    /**
     * @brief Copying Passkey is forbidden.
     *
     * @param rhs Unused.
     */
    Passkey(const Passkey &rhs) = delete;
    /**
     * @brief Assigning Passkey is forbidden.
     *
     * @param rhs Unused.
     *
     * @returns Unused.
     */
    Passkey & operator=(const Passkey &rhs) = delete;
};

/**
 * @brief Handy Passkey shortcut.
 *
 * @tparam T Type that has the permission to use the interface.
 */
template <typename T>
using pk = Passkey<T>;

#endif // SCRIBE__UTILS__PASSKEY_HPP__