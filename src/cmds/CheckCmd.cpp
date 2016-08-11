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

// TODO: more things to check:
// * Check that all sequences are of the size of the alphabet, which is not
//   zero.
// * Order in which items were created should match the sequence?
// * Validate contents of items (timestamps, duplicates, redundancy, conflicts).
// * Validate names of item fields.

#include <cstdlib>

#include <algorithm>
#include <functional>
#include <iterator>
#include <set>
#include <vector>

#include "utils/contains.hpp"
#include "Commands.hpp"
#include "IdGenerator.hpp"
#include "Item.hpp"
#include "Project.hpp"
#include "Storage.hpp"

namespace {

/**
 * @brief Implementation of "check" command, which verifies project.
 */
class CheckCmd : public AutoRegisteredCommand<CheckCmd>
{
public:
    /**
     * @brief Constructs the command implementation.
     */
    CheckCmd();

public:
    /**
     * @copydoc Command::run()
     */
    virtual boost::optional<int> run(
        Project &project,
        const std::vector<std::string> &args) override;

private:
    /**
     * @brief Compares actual list of ids with expected list.
     *
     * @param items Known items.
     * @param idGenerator Generator of item IDs.
     *
     * @returns Exit code like one returned by @c main().
     */
    int checkIdList(const std::vector<std::reference_wrapper<Item>> &items,
                    IdGenerator &idGenerator);
};

}

CheckCmd::CheckCmd()
    : parent("check", "verify project state", "Usage: check")
{
}

boost::optional<int>
CheckCmd::run(Project &project, const std::vector<std::string> &)
{
    Storage &storage = project.getStorage();
    IdGenerator &idGenerator = storage.getIdGenerator();

    std::vector<std::reference_wrapper<Item>> items = storage.list();

    // Check that number of items equals "total" in configuration.
    const int total = items.size();
    if (total != idGenerator.size()) {
        out() << "Unexpected number of items: " << total << " instead of "
              << idGenerator.size() << '\n';
        return EXIT_FAILURE;
    }

    return checkIdList(items, idGenerator);
}

int
CheckCmd::checkIdList(const std::vector<std::reference_wrapper<Item>> &items,
                      IdGenerator &idGenerator)
{
    int result = EXIT_SUCCESS;

    std::set<std::string> actualIds;
    std::transform(items.cbegin(), items.cend(),
                   std::inserter(actualIds, actualIds.begin()),
                   [](const Item &item) {
                       return item.getId();
                   });

    // List absent elements.
    std::set<std::string> expectedIds;
    idGenerator.forEachId(
        [this, &actualIds, &expectedIds, &result](const std::string &id) {
            expectedIds.insert(id);
            if (!contains(actualIds, id)) {
                out() << "Missing item with id: " << id << '\n';
                result = EXIT_FAILURE;
            }
        });

    // List extra elements.
    std::vector<std::string> extraIds;
    std::set_difference(actualIds.cbegin(), actualIds.cend(),
                        expectedIds.cbegin(), expectedIds.cend(),
                        std::back_inserter(extraIds));
    for (const std::string &id : extraIds) {
        out() << "Extra item with id: " << id << '\n';
        result = EXIT_FAILURE;
    }

    return result;
}
