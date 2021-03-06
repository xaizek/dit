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

#include "completion.hpp"

#include <cstdlib>

#include <ostream>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include <boost/range/iterator_range.hpp>
#include <boost/filesystem.hpp>

#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"

std::vector<std::string>
listProjects(const std::string &projectsDir)
{
    namespace fs = boost::filesystem;

    std::vector<std::string> names;

    using dir_it = fs::directory_iterator;
    for (fs::directory_entry &e :
         boost::make_iterator_range(dir_it(projectsDir), dir_it())) {
        if (Project(e.path().string()).exists()) {
            names.push_back(e.path().filename().string());
        }
    }

    return names;
}

int
completeIds(Storage &storage, std::ostream &os)
{
    for (Item &item : storage.list()) {
        os << item.getId() << '\n';
    }
    return EXIT_SUCCESS;
}

int
completeKeys(Item &item, std::ostream &os, const std::vector<std::string> &args)
{
    std::set<std::string> keys = item.listRecordNames();

    // Remove elements already present on the command-line from completion list.
    for (const std::string &arg : args) {
        keys.erase(arg);
    }

    for (const std::string &key : keys) {
        os << key << '\n';
    }

    return EXIT_SUCCESS;
}

int
completeKeys(Storage &storage, std::ostream &os)
{
    std::set<std::string> keys;

    for (Item &item : storage.list()) {
        const std::set<std::string> &itemKeys = item.listRecordNames();
        keys.insert(itemKeys.cbegin(), itemKeys.cend());
    }

    for (const std::string &key : keys) {
        os << key << '\n';
    }

    return EXIT_SUCCESS;
}

int
completeKeys(Storage &storage, std::ostream &os,
             const std::vector<std::string> &args)
{
    std::set<std::string> keys;

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

int
completeValues(Storage &storage, std::ostream &os, const std::string &key)
{
    std::set<std::string> values;

    for (Item &item : storage.list()) {
        std::string value = item.getValue(key);
        if (!value.empty()) {
            values.insert(std::move(value));
        }
    }

    for (const std::string &value : values) {
        os << value << '\n';
    }

    return EXIT_SUCCESS;
}
