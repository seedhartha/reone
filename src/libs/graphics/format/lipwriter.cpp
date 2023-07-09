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

#include "reone/graphics/format/lipwriter.h"

#include "reone/system/binarywriter.h"
#include "reone/system/stream/fileoutput.h"

namespace reone {

namespace graphics {

LipWriter::LipWriter(LipAnimation &&animation) :
    _animation(animation) {
}

void LipWriter::save(const boost::filesystem::path &path) {
    auto lip = std::make_shared<FileOutputStream>(path);

    BinaryWriter writer(*lip);
    writer.writeString("LIP V1.0");
    writer.writeFloat(_animation.length());
    writer.writeUint32(static_cast<uint32_t>(_animation.keyframes().size()));
    for (auto &keyframe : _animation.keyframes()) {
        writer.writeFloat(keyframe.time);
        writer.writeByte(keyframe.shape);
    }
}

} // namespace graphics

} // namespace reone
