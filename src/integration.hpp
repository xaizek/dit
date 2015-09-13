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

#ifndef SCRIBE__INTEGRATION_HPP__
#define SCRIBE__INTEGRATION_HPP__

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

#endif // SCRIBE__INTEGRATION_HPP__
