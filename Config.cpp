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

#include "Config.hpp"

#include <string>
#include <utility>
#include <vector>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/filesystem.hpp>

#include "utils/propsRange.hpp"

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

Config::Config(std::string path) : path(std::move(path)), changed(false)
{
}

std::string
Config::get(const std::string &key)
{
    ensureLoaded();
    return props.get<std::string>(key);
}

std::string
Config::get(const std::string &key, const std::string &def)
{
    ensureLoaded();
    std::string val = props.get<std::string>(key, def);
    return val.empty() ? def : std::move(val);
}

std::vector<std::string>
Config::list()
{
    ensureLoaded();

    std::vector<std::string> list;
    for (const std::string &propName : propsRange(props)) {
        if (propName[0] != '!' && !get(propName).empty()) {
            list.push_back(propName);
        }
    }
    return std::move(list);
}

void
Config::set(const std::string &key, const std::string &val)
{
    ensureLoaded();

    try {
        if (get(key) == val) {
            return;
        }
    } catch (pt::ptree_bad_path &) {
        // Skip the check if field doesn't exist.
    }

    props.put(key, val);
    changed = true;
}

void
Config::save()
{
    if (!changed) {
        return;
    }

    write_info(path, props);
}

void
Config::load()
{
    try {
        read_info(path, props);
    } catch (pt::info_parser_error &) {
        // It's OK if specific configuration doesn't exist yet.
        if (fs::exists(path)) {
            throw;
        }
    }
}
