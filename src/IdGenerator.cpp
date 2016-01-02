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

#include "IdGenerator.hpp"

#include <cassert>
#include <cmath>

#include <algorithm>
#include <array>
#include <random>
#include <string>
#include <utility>

#include "Config.hpp"

static int getIdx(int k, int b);

/**
 * @brief Set of characters available for use in sequences.
 */
static const char ALPHABET[] =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

void
IdGenerator::init(Config &config)
{
    std::array<std::string, 3> sequences = { ALPHABET, ALPHABET, ALPHABET };

    std::random_device rd;
    std::mt19937 g(rd());

    for (std::string &s : sequences) {
        std::shuffle(s.begin(), s.end(), g);
    }

    config.set("!ids.sequences.first", sequences[0]);
    config.set("!ids.sequences.second", sequences[1]);
    config.set("!ids.sequences.third", sequences[2]);
    config.set("!ids.count", "0");
    config.set("!ids.next",
               { sequences[0][0], sequences[1][0], sequences[2][0] });
}

IdGenerator::IdGenerator(Config &config) : config(config)
{
}

std::string
IdGenerator::getId()
{
    ensureLoaded();

    return nextId;
}

void
IdGenerator::advanceId()
{
    ensureLoaded();

    nextId = advance(nextId, count);
    ++count;

    // TODO: at least report on reaching upper limit for IDs, or better extend
    //       numberation by one field.

    markModified();
}

void
IdGenerator::load()
{
    sequences[0] = config.get("!ids.sequences.first");
    sequences[1] = config.get("!ids.sequences.second");
    sequences[2] = config.get("!ids.sequences.third");

    assert(sequences[0].size() == sequences[1].size() && "Lengths must match.");
    assert(sequences[0].size() == sequences[2].size() && "Lengths must match.");
    assert(sequences[1].size() == sequences[2].size() && "Lengths must match.");
    assert(!sequences[0].empty() && "Empty sequences.");

    nextId = config.get("!ids.next");
    count = std::stoi(config.get("!ids.count"));
}

std::string
IdGenerator::advance(std::string id, int count) const
{
    const int i = getIdx(count + 1, sequences[0].size());
    std::string::size_type x = sequences[i].find(id[i]);
    assert(x != std::string::npos && "Wrong character in id.");
    x = (x + 1)%sequences[0].size();
    id[i] = sequences[i][x];
    return std::move(id);
}

/**
 * @brief Calculates number of the field to advance to get the next id.
 *
 * @param k Sequential number of the ID (base 1).
 * @param b Number base (number of "digits" per field).
 *
 * @returns The field number.
 */
static int
getIdx(int k, int b)
{
    if (k == 0) {
        return 0;
    }

    int p = std::ceil(std::log(k)/std::log(b));
    int i = std::pow(b, p);
    while (p > 0) {
        if (k % i == 0) {
            break;
        }
        i /= b;
        --p;
    }
    return p;
}

void
IdGenerator::save()
{
    if (!isModified()) {
        return;
    }

    config.set("!ids.sequences.first", sequences[0]);
    config.set("!ids.sequences.second", sequences[1]);
    config.set("!ids.sequences.third", sequences[2]);
    config.set("!ids.next", nextId);
    config.set("!ids.count", std::to_string(count));
}
