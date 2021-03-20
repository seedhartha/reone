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

#include "erfwriter.h"

#include <memory>

#include <boost/filesystem.hpp>

#include "../../common/streamwriter.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static constexpr int kKeyStructSize = 24;
static constexpr int kResourceStructSize = 8;

void ErfWriter::add(Resource &&res) {
    _resources.push_back(res);
}

// Adapted from Bioware ERF specification
void ErfWriter::save(FileType type, const fs::path &path) {
    auto rim = make_shared<fs::ofstream>(path, ios::binary);
    StreamWriter writer(rim);
    auto numResources = static_cast<uint32_t>(_resources.size());
    uint32_t offResources = 0xa0 + kKeyStructSize * numResources;

    if (type == FileType::MOD) {
        writer.putString("MOD V1.0");
    } else {
        writer.putString("ERF V1.0");
    }
    writer.putUint32(0); // language count
    writer.putUint32(0); // localized string size
    writer.putUint32(numResources);
    writer.putUint32(0xa0); // offset to localized string
    writer.putUint32(0xa0); // offset to key list
    writer.putUint32(offResources); // offset to resource list
    writer.putUint32(0); // build year since 1900
    writer.putUint32(0); // build day since January 1st
    writer.putBytes(120); // padding

    uint32_t id = 0;

    // Write keys
    for (auto &res : _resources) {
        string resRef(res.resRef);
        resRef.resize(16);
        writer.putString(resRef);

        writer.putUint32(id++);
        writer.putUint32(static_cast<uint16_t>(res.resType));
        writer.putUint16(0); // unused
    }

    uint32_t offset = offResources + (kKeyStructSize + kResourceStructSize) * numResources;

    // Write resources
    for (auto &res : _resources) {
        auto size = static_cast<uint32_t>(res.data.size());
        writer.putUint32(offset);
        writer.putUint32(size);
        offset += size;
    }

    // Write resource data
    for (auto &res : _resources) {
        writer.putBytes(res.data);
    }
}

} // namespace resource

} // namespace reone
