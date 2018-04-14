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

#ifndef DIT__UTILS__PASSKEY_HPP__
#define DIT__UTILS__PASSKEY_HPP__

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
     *
     * This must not be defaulted as compiler will treat this class as an
     * aggregate and allow construction with {} even with constructor being
     * private.
     */
    Passkey() {}

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

#endif // DIT__UTILS__PASSKEY_HPP__
