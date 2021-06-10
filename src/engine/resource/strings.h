/*
 * Copyright (c) 2020-2021 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "pch.h"
#include "format/tlkreader.h"

#include "types.h"

using namespace std;

namespace reone {

namespace resource {

class Strings {
public:
    Strings() = default;

    void init(const boost::filesystem::path &gameDir);

    /**
     * Searches for a string in the global talktable by StrRef.
     *
     * @return string from the global talktable if found, empty string otherwise
     */
    std::string get(int strRef);

    /**
     * Searches for a sound in the global talktable by StrRef.
     *
     * @return ResRef of a sound from the global talktable if found, empty string otherwise
     */
    std::string getSound(int strRef);

private:
    TlkReader _tlk;

    void process(std::string &str);
    void stripDeveloperNotes(std::string &str);
};

} // namespace resource

} // namespace reone
