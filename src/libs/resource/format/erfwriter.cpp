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

#include "reone/resource/format/erfwriter.h"

#include "reone/system/binarywriter.h"
#include "reone/system/stream/fileoutput.h"

namespace reone {

namespace resource {

static constexpr int kKeyStructSize = 24;
static constexpr int kResourceStructSize = 8;

void ErfWriter::add(Resource &&res) {
    _resources.push_back(res);
}

void ErfWriter::save(FileType type, const boost::filesystem::path &path) {
    auto out = FileOutputStream(path, OpenMode::Binary);
    save(type, out);
}

void ErfWriter::save(FileType type, IOutputStream &out) {
    BinaryWriter writer(out);
    auto numResources = static_cast<uint32_t>(_resources.size());
    uint32_t offResources = 0xa0 + kKeyStructSize * numResources;

    if (type == FileType::MOD) {
        writer.writeString("MOD V1.0");
    } else {
        writer.writeString("ERF V1.0");
    }
    writer.writeUint32(0); // language count
    writer.writeUint32(0); // localized std::string size
    writer.writeUint32(numResources);
    writer.writeUint32(0xa0);         // offset to localized string
    writer.writeUint32(0xa0);         // offset to key list
    writer.writeUint32(offResources); // offset to resource list
    writer.writeUint32(0);            // build year since 1900
    writer.writeUint32(0);            // build day since January 1st
    writer.writeInt32(-1);            // StrRef for file description
    writer.writeBytes(116);           // padding

    uint32_t id = 0;

    // Write keys
    for (auto &res : _resources) {
        std::string resRef(res.resRef);
        resRef.resize(16);
        writer.writeString(resRef);

        writer.writeUint32(id++);
        writer.writeUint16(static_cast<uint16_t>(res.resType));
        writer.writeUint16(0); // unused
    }

    uint32_t offset = 0xa0 + (kKeyStructSize + kResourceStructSize) * numResources;

    // Write resources
    for (auto &res : _resources) {
        auto size = static_cast<uint32_t>(res.data.size());
        writer.writeUint32(offset);
        writer.writeUint32(size);
        offset += size;
    }

    // Write resource data
    for (auto &res : _resources) {
        writer.writeBytes(res.data);
    }
}

} // namespace resource

} // namespace reone
