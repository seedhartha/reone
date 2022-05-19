/*
 * Copyright (c) 2020-2022 The reone project contributors
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

namespace reone {

namespace resource {

class TalkTable : boost::noncopyable {
public:
    struct String {
        std::string text;
        std::string soundResRef;
    };

    TalkTable(std::vector<String> strings) :
        _strings(std::move(strings)) {
    }

    int getStringCount() const;
    const String &getString(int index) const;

private:
    std::vector<String> _strings;
};

} // namespace resource

} // namespace reone
