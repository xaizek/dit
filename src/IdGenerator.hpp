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

#ifndef SCRIBE__IDGENERATOR_HPP__
#define SCRIBE__IDGENERATOR_HPP__

#include <array>
#include <string>

#include "LazyLoadable.hpp"

class Config;

/**
 * @brief Manages IDs for items by generating the next unique one.
 */
class IdGenerator : private LazyLoadable<IdGenerator>
{
    friend class LazyLoadable<IdGenerator>;

public:
    /**
     * @brief Creates generator for the configuration.
     *
     * @param config Configuration to initialize.
     */
    static void init(Config &config);

public:
    /**
     * @brief Constructs ID generator out of configuration.
     *
     * @param config
     */
    IdGenerator(Config &config);

public:
    /**
     * @brief Retrieves current unemployed ID.
     *
     * @returns The id.
     */
    std::string getId();
    /**
     * @brief Signifies that value returned by getId() was employed.
     *
     * As a result generates next ID.
     */
    void advanceId();
    /**
     * @brief Stores changed state into configuration.
     */
    void save();

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
     * @returns Next ID.
     */
    std::string advance(std::string id, int count) const;

private:
    /**
     * @brief Storage for the input/state.
     */
    Config &config;
    /**
     * @brief ID position sequences.
     */
    std::array<std::string, 3> sequences;
    /**
     * @brief Next id to be issued.
     */
    std::string nextId;
    /**
     * @brief Number of already issues IDs.
     */
    int count;
    /**
     * @brief Whether there is a state to be stored.
     */
    bool changed;
};

#endif // SCRIBE__IDGENERATOR_HPP__
