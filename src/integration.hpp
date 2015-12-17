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

#ifndef DIT__INTEGRATION_HPP__
#define DIT__INTEGRATION_HPP__

#include <string>

#include <boost/optional.hpp>

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
 * @brief Retrieves terminal width.
 *
 * @returns Actual terminal width, or maximum possible value allowed by type.
 */
unsigned int getTerminalWidth();

#endif // DIT__INTEGRATION_HPP__
