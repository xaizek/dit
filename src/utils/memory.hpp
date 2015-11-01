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

#ifndef SCRIBE__UTILS__MEMORY_HPP__
#define SCRIBE__UTILS__MEMORY_HPP__

#include <memory>

/**
 * @brief make_unique function missing in C++11.
 *
 * @tparam T Type of object to construct.
 * @tparam Args Types of constructor arguments.
 * @param args Actual arguments for the constructor.
 *
 * @returns Unique pointer with newly constructed object.
 */
template <typename T, typename... Args>
inline std::unique_ptr<T>
make_unique(Args... args)
{
    return std::unique_ptr<T>(new T(args...));
}

#endif // SCRIBE__UTILS__MEMORY_HPP__
