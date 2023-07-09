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

#include "reone/resource/format/rimwriter.h"

#include "reone/system/binarywriter.h"
#include "reone/system/stream/fileoutput.h"

namespace reone {

namespace resource {

void RimWriter::add(Resource &&res) {
    _resources.push_back(res);
}

void RimWriter::save(const std::filesystem::path &path) {
    auto rim = FileOutputStream(path);
    save(rim);
}

void RimWriter::save(IOutputStream &out) {
    BinaryWriter writer(out);
    uint32_t numResources = static_cast<uint32_t>(_resources.size());

    writer.writeString("RIM V1.0");
    writer.writeUint32(0); // reserved
    writer.writeUint32(numResources);
    writer.writeUint32(0x78);  // offset to resource headers
    writer.write(100, 0); // reserved

    uint32_t id = 0;
    uint32_t offset = 0x78 + numResources * 32;

    // Write resource headers
    for (auto &res : _resources) {
        auto size = static_cast<uint32_t>(res.data.size());

        std::string resRef(res.resRef);
        resRef.resize(16);
        writer.writeString(resRef);

        writer.writeUint32(static_cast<uint32_t>(res.resType));
        writer.writeUint32(id++);
        writer.writeUint32(offset);
        writer.writeUint32(size);

        offset += size;
    }

    // Write resources data
    for (auto &res : _resources) {
        writer.write(res.data);
    }
}

} // namespace resource

} // namespace reone
