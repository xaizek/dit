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

// Initial width of an ID is three.  It starts with count zero for ID sequence
// of width three.  Each time end of current sequence is reached, the width is
// incremented by one and count is reset back to zero.

#include "IdGenerator.hpp"

#include <cassert>
#include <cmath>

#include <algorithm>
#include <array>
#include <random>
#include <string>
#include <tuple>
#include <utility>

#include "Config.hpp"

static int getIdx(int k, int b);
template <typename C>
inline C shuffle(const C &c);

void
IdGenerator::init(Config &config, const std::string &alphabet)
{
    // Initial number of sequences (width of an ID) is three.
    std::array<std::string, 3> sequences = {
        shuffle(alphabet), shuffle(alphabet), shuffle(alphabet)
    };

    config.set("!ids.sequences.alphabet", alphabet);
    config.set("!ids.sequences.count", std::to_string(sequences.size()));
    config.set("!ids.sequences.0", sequences[0]);
    config.set("!ids.sequences.1", sequences[1]);
    config.set("!ids.sequences.2", sequences[2]);
    config.set("!ids.count", "0");
    config.set("!ids.total", "0");
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

    std::tie(nextId, count) = advance(nextId, count);
    ++total;

    markModified();
}

void
IdGenerator::load()
{
    alphabet = config.get("!ids.sequences.alphabet");
    const int nseq = std::stoi(config.get("!ids.sequences.count"));
    for (int seq = 0; seq < nseq; ++seq) {
        sequences.push_back(
            config.get("!ids.sequences." + std::to_string(seq)));
    }

    nextId = config.get("!ids.next");
    count = std::stoi(config.get("!ids.count"));
    total = std::stoi(config.get("!ids.total"));
}

std::pair<std::string, int>
IdGenerator::advance(std::string id, int count)
{
    const int i = getIdx(count + 1, sequences[0].size());
    if (static_cast<unsigned int>(i) == id.length()) {
        sequences.push_back(shuffle(alphabet));

        // Make id from first characters of all sequences and reset count to
        // zero.
        id.clear();
        for (const std::string &seq : sequences) {
            id.push_back(seq.front());
        }
        return { std::move(id), 0 };
    }

    std::string::size_type x = sequences[i].find(id[i]);
    assert(x != std::string::npos && "Wrong character in id.");
    x = (x + 1)%sequences[0].size();
    id[i] = sequences[i][x];
    return { std::move(id), count + 1 };
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
    // No check for k == 0 followed by returning zero as we never have such
    // input.

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

/**
 * @brief Shuffles elements of the given container.
 *
 * @tparam C Type of the container.
 * @param c Container itself.
 *
 * @returns Copy of the container with shuffled items.
 */
template <typename C>
inline C shuffle(const C &c)
{
    C shuffled = c;

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffled.begin(), shuffled.end(), g);

    return std::move(shuffled);
}

void
IdGenerator::save()
{
    if (!isModified()) {
        return;
    }

    config.set("!ids.sequences.alphabet", alphabet);
    config.set("!ids.next", nextId);
    config.set("!ids.count", std::to_string(count));
    config.set("!ids.total", std::to_string(total));

    config.set("!ids.sequences.count", std::to_string(sequences.size()));
    for (unsigned int seq = 0U; seq < sequences.size(); ++seq) {
        config.set("!ids.sequences." + std::to_string(seq), sequences[seq]);
    }
}
