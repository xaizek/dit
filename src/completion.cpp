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

#include "completion.hpp"

#include <cstdlib>

#include <ostream>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "Item.hpp"
#include "Storage.hpp"

int
completeKeys(Storage &storage, std::ostream &os,
             const std::vector<std::string> &args)
{
    std::unordered_set<std::string> keys;

    for (Item &item : storage.list()) {
        const std::set<std::string> &itemKeys = item.listRecordNames();
        keys.insert(itemKeys.cbegin(), itemKeys.cend());
    }

    // Remove elements already present on the command-line from completion list.
    for (const std::string &arg : args) {
        const std::string::size_type pos = arg.find('=');
        if (pos != 0U && pos != std::string::npos) {
            keys.erase(arg.substr(0U, pos));
            continue;
        }

        if (!arg.empty() && arg.back() == ':') {
            keys.erase(arg.substr(0U, arg.size() - 1U));
        }
    }

    // Due to implicitly added space after completion match colon form is easier
    // to type.
    for (const std::string &key : keys) {
        os << key << ":\n";
    }

    return EXIT_SUCCESS;
}
