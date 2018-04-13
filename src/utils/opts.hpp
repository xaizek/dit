// Copyright (C) 2016 xaizek <xaizek@posteo.net>
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

#ifndef DIT__UTILS__OPTS_HPP__
#define DIT__UTILS__OPTS_HPP__

#include <string>
#include <vector>

namespace boost { namespace program_options {
    class options_description;
    class variables_map;
}}

/**
 * @brief Parses command-specific options.
 *
 * Positional options are accessible via "positional" key.
 *
 * @param args List of options.
 * @param opts Option specification
 *
 * @returns Mapping of options into actually passed values.
 */
boost::program_options::variables_map
parseOpts(const std::vector<std::string> &args,
          const boost::program_options::options_description &opts);

#endif // DIT__UTILS__OPTS_HPP__
