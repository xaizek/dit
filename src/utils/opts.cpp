// Copyright (C) 2016 xaizek <xaizek@openmailbox.org>
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

#include "opts.hpp"

#include <boost/program_options.hpp>

#include <string>
#include <vector>

namespace po = boost::program_options;

po::variables_map
parseOpts(const std::vector<std::string> &args,
          const po::options_description &opts)
{
    po::options_description hiddenOpts;
    hiddenOpts.add_options()
        ("positional", po::value<std::vector<std::string>>(),
         "positional args");

    po::options_description all;
    all.add(opts).add(hiddenOpts);

    po::positional_options_description p;
    p.add("positional", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(args).options(all).positional(p).run(),
              vm);
    po::notify(vm);
    return vm;
}
