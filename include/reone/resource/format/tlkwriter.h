/*
 * Copyright (c) 2020-2023 The reone project contributors
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

class IOutputStream;

namespace resource {

class TalkTable;

class TlkWriter : boost::noncopyable {
public:
    TlkWriter(TalkTable &talkTable) :
        _talkTable(talkTable) {
    }

    void save(const std::filesystem::path &path);
    void save(IOutputStream &out);

private:
    struct StringDataElement {
        std::string soundResRef;
        uint32_t offString {0};
        uint32_t stringSize {0};
    };

    TalkTable &_talkTable;
};

} // namespace resource

} // namespace reone
