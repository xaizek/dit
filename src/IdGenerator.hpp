// Copyright (C) 2015 xaizek <xaizek@posteo.net>
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

#ifndef DIT__IDGENERATOR_HPP__
#define DIT__IDGENERATOR_HPP__

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "StorageBacked.hpp"

class Config;

/**
 * @brief Manages IDs for items by generating the next unique one.
 */
class IdGenerator : private StorageBacked<IdGenerator>
{
    friend class StorageBacked<IdGenerator>;

public:
    /**
     * @brief Creates generator for the configuration.
     *
     * @param config Configuration to initialize.
     * @param alphabet Set of characters available for use in sequences.
     */
    static void init(Config &config, const std::string &alphabet);

public:
    /**
     * @brief Constructs ID generator out of configuration.
     *
     * @param config Configuration to use for reading/saving state.
     */
    explicit IdGenerator(Config &config);

public:
    /**
     * @brief Retrieves current unemployed ID.
     *
     * @returns The ID.
     */
    std::string getId();
    /**
     * @brief Signifies that value returned by getId() was employed.
     *
     * As a result generates next ID.
     */
    void advanceId();

    /**
     * @brief Retrieves size of the generated sequence of IDs so far.
     *
     * @returns The size.
     */
    int size() { ensureLoaded(); return total; }

    /**
     * @brief Runs visitor once for each ID in order of their generation.
     *
     * @param visitor Visitor to invoke per ID.
     */
    void forEachId(std::function<void(const std::string &)> visitor);

    /**
     * @brief Stores changed state into configuration.
     */
    virtual void save() override;

private:
    /**
     * @brief Loads data from configuration.
     */
    void load();
    /**
     * @brief Turns ID into the next one.
     *
     * Source: The Art of Programming, IV, 7.2.1.1.
     *
     * @param id Current ID.
     * @param count Sequential number of current ID.
     *
     * @returns Pair of next ID and new count.
     */
    std::pair<std::string, int> advance(std::string id, int count);
    /**
     * @brief Dumps current state into configuration object.
     *
     * @param config Configuration to save data into.
     */
    void dump(Config &config) const;

private:
    /**
     * @brief Storage for the input/state.
     */
    Config &config;
    /**
     * @brief ID position sequences.
     */
    std::vector<std::string> sequences;
    /**
     * @brief Next ID to be issued.
     */
    std::string nextId;
    /**
     * @brief Count within ID sequence of current width.
     */
    int count;
    /**
     * @brief Total number of already issued IDs.
     */
    int total;
    /**
     * @brief Alphabet used in this particular instance of the generator.
     */
    std::string alphabet;
};

#endif // DIT__IDGENERATOR_HPP__
