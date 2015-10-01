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

#ifndef SCRIBE__UTILS__FS_HPP__
#define SCRIBE__UTILS__FS_HPP__

#include <cstdlib>

#include <string>

#include <boost/filesystem/operations.hpp>

/**
 * @brief Temporary file in RAII-style.
 */
class TempFile
{
public:
    /**
     * @brief Makes temporary file, which is removed in destructor.
     *
     * @param prefix File name prefix.
     */
    explicit TempFile(const std::string &prefix)
    {
        namespace fs = boost::filesystem;

        path = (fs::temp_directory_path()
             /  fs::unique_path("scribe-" + prefix + "-%%%%-%%%%")).string();
    }

    /**
     * @brief Removes temporary file, if it still exists.
     */
    ~TempFile()
    {
        static_cast<void>(std::remove(path.c_str()));
    }

public:
    /**
     * @brief Provides implicit convertion to a file path string.
     *
     * @returns The path.
     */
    operator std::string() const
    {
        return path;
    }

private:
    /**
     * @brief Path to the temporary file.
     */
    std::string path;
};

#endif // SCRIBE__UTILS__FS_HPP__
