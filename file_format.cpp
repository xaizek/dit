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

#include "file_format.hpp"

#include <cctype>
#include <ctime>

#include <istream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "Change.hpp"
#include "utils.hpp"

static std::pair<std::string, std::string> splitRecord(const std::string &s);
static std::string decode(std::string str);

std::istream &
operator>>(std::istream &s, std::vector<Change> &changes)
{
    using boost::lexical_cast;

    std::time_t timestamp;
    bool timestampSet = false;
    for (const std::string &l : getLines(s)) {
        if (std::isdigit(l[0])) {
            timestamp = lexical_cast<std::time_t>(l);
            timestampSet = true;
            continue;
        }

        if (!timestampSet) {
            throw std::runtime_error("Wrong field ordering, no timestamp");
        }

        std::string key, val;
        std::tie(key, val) = splitRecord(l);

        changes.emplace_back(timestamp, key, val);
    }

    return s;
}

/**
 * @brief Splits single entry string into key and value pair.
 *
 * @param s String of the form `key=encoded-value`
 *
 * @returns Key and decoded value pair.
 */
static std::pair<std::string, std::string>
splitRecord(const std::string &s)
{
    const std::string::size_type pos = s.find('=');
    if (pos == std::string::npos) {
        throw std::runtime_error("Broken item field: " + s);
    }

    return { s.substr(0, pos), decode(s.substr(pos + 1U)) };
}

/**
 * @brief Decodes value restoring its original content.
 *
 * @param str Encoded data.
 *
 * @returns Decoded data.
 */
static std::string
decode(std::string str)
{
    boost::replace_all(str, R"(\n)", "\n");
    boost::replace_all(str, R"(\\)", R"(\)");
    return std::move(str);
}
