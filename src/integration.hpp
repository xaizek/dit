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

#ifndef DIT__INTEGRATION_HPP__
#define DIT__INTEGRATION_HPP__

#include <memory>
#include <string>

#include <boost/optional.hpp>

/**
 * @brief A class that automatically spawns pager if output is large.
 *
 * Output must come to @c std::cout and is considered to be large when it
 * doesn't fit screen height.
 */
class RedirectToPager
{
public:
    class Impl;

    /**
     * @brief Can redirect @c std::cout until destruction.
     *
     * @param pagerCmd Command to invoke a pager.
     */
    explicit RedirectToPager(const std::string &pagerCmd);

    //! No copy-constructor.
    RedirectToPager(const RedirectToPager &rhs) = delete;
    //! No copy-move.
    RedirectToPager & operator=(const RedirectToPager &rhs) = delete;

    /**
     * @brief Restores previous state of @c std::cout.
     */
    ~RedirectToPager();

private:
    //! Implementation details.
    std::unique_ptr<Impl> impl;
};

/**
 * @brief Edits value if user requested for it.
 *
 * @param key Key (name of the value).
 * @param value New value (can be special value, which means "edit").
 * @param current Current value to be edited.
 *
 * @returns Nothing if @p value can be used as is, otherwise new value.
 */
boost::optional<std::string> editValue(const std::string &key,
                                       const std::string &value,
                                       const std::string &current);

/**
 * @brief Queries whether program output is connected to terminal.
 *
 * @returns @c true if so, otherwise @c false.
 */
bool isOutputToTerminal();

/**
 * @brief Retrieves terminal width.
 *
 * @returns Actual terminal width, or maximum possible value allowed by type.
 */
unsigned int getTerminalWidth();

#endif // DIT__INTEGRATION_HPP__
