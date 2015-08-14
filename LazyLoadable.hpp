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

#ifndef SCRIBE__LAZYLOADABLE_HPP__
#define SCRIBE__LAZYLOADABLE_HPP__

/**
 * @brief Helper template that follows CRTP to provide lazy loading.
 *
 * The derived class should either inherit from this one publicly or "friend"
 * it.
 *
 * @tparam D Type of derived class.
 */
template <class D>
class LazyLoadable
{
protected:
    /**
     * @brief Initializes the instance as "not loaded".
     */
    LazyLoadable() : loaded(false)
    {
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

private:
    /**
     * @brief Whether configuration was loaded from permanent storage.
     */
    bool loaded;
};

#endif // SCRIBE__LAZYLOADABLE_HPP__
