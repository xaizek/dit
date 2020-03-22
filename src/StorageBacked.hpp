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

#ifndef DIT__STORAGEBACKED_HPP__
#define DIT__STORAGEBACKED_HPP__

#include <cassert>

/**
 * @brief Represents data that is loaded from and stored to external storage.
 *
 * Follows CRTP.  The derived class should either inherit from this one publicly
 * or "friend" it.
 *
 * @tparam D Type of derived class.
 */
template <class D>
class StorageBacked
{
protected:
    /**
     * @brief Initializes the instance as "not loaded" (by default).
     *
     * "loaded" objects are useful when creating something that is not yet in
     * the storage and for tests.
     *
     * @param loaded Whether instance is loaded on initialization.
     */
    StorageBacked(bool loaded = false) : loaded(loaded), modified(false)
    {
    }

public:
    /**
     * @brief Stores changes.
     *
     * Some objects can delegate saving them to other objects, so empty default
     * implementation makes sense.
     */
    virtual void save()
    {
        assert(false &&
               "Saving derived instance that doesn't implement saving.");
    }

protected:
    /**
     * @brief Ensures that in-memory representation is up-to-date.
     */
    void ensureLoaded()
    {
        if (!loaded) {
            static_cast<D*>(this)->load();
            loaded = true;
        }
    }
    /**
     * @brief Checks whether host is already loaded.
     *
     * @returns @c true if so, @c false otherwise.
     */
    bool isLoaded() const
    {
        return loaded;
    }

    /**
     * @brief Marks data as modified.
     */
    void markModified()
    {
        modified = true;
    }
    /**
     * @brief Retrieves whether there are any modifications worth saving.
     *
     * @returns @c true if so, otherwise @c false is returned.
     */
    bool isModified() const
    {
        return modified;
    }

private:
    /**
     * @brief Whether configuration was loaded from permanent storage.
     */
    bool loaded;
    /**
     * @brief Whether storage was modified (and might need saving).
     */
    bool modified;
};

#endif // DIT__STORAGEBACKED_HPP__
