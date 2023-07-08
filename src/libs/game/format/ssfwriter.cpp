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

#include "reone/game/format/ssfwriter.h"

#include "reone/system/binarywriter.h"
#include "reone/system/stream/fileoutput.h"

namespace reone {

namespace game {

void SsfWriter::save(const boost::filesystem::path &path) {
    auto stream = FileOutputStream(path, OpenMode::Binary);
    auto writer = BinaryWriter(stream);

    writer.writeString("SSF V1.1");
    writer.writeUint32(12); // offset to entries

    for (auto val : _soundSet) {
        writer.writeUint32(val);
    }
}

} // namespace game

} // namespace reone
