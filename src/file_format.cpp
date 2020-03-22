// Copyright (C) 2015 xaizek <xaizek@posteo.net>
//
// This file is part of dit.
//
// dit is free software: you can redistribute it and/or modify
// it under the terms of version 3 of the GNU Affero General Public
// License as published by the Free Software Foundation.
//
// dit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dit.  If not, see <http://www.gnu.org/licenses/>.

#include "file_format.hpp"

#include <cctype>
#include <ctime>

#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>

#include "utils/getLines.hpp"
#include "utils/strings.hpp"
#include "Change.hpp"

static std::pair<std::string, std::string> splitRecord(const std::string &s);
static std::string decode(std::string str);
static std::string encode(std::string str);

std::istream &
operator>>(std::istream &s, std::vector<Change> &changes)
{
    using boost::lexical_cast;

    std::time_t timestamp;
    bool timestampSet = false;
    for (const std::string &l : getLines(s)) {
        if (l.empty()) {
            throw std::runtime_error("Empty strings are not expected");
        }

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
    const std::pair<std::string, std::string> p = splitAt(s, '=');
    return { p.first, decode(p.second) };
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
    return str;
}

std::ostream &
operator<<(std::ostream &s, const std::vector<Change> &changes)
{
    if (changes.empty()) {
        return s;
    }

    std::time_t timestamp = changes.front().getTimestamp() + 1;
    for (const Change &c : changes) {
        const std::time_t ts = c.getTimestamp();
        if (ts != timestamp) {
            s << ts << '\n';
            timestamp = ts;
        }

        s << c.getKey() << '=' << encode(c.getValue()) << '\n';
    }

    return s;
}

/**
 * @brief Encodes value to make it single-line string.
 *
 * @param str Data.
 *
 * @returns Encoded data.
 */
static std::string
encode(std::string str)
{
    boost::replace_all(str, R"(\)", R"(\\)");
    boost::replace_all(str, "\n", R"(\n)");
    return str;
}
