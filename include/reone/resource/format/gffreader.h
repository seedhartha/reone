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

#include "reone/system/binaryreader.h"
#include "reone/system/stream/input.h"

#include "../gff.h"

namespace reone {

namespace resource {

class GffReader : boost::noncopyable {
public:
    GffReader(IInputStream &gff) :
        _gff(gff) {
    }

    void load();

    std::shared_ptr<Gff> root() const {
        return _root;
    }

private:
    struct LocString {
        int32_t strRef {-1};
        std::string subString;
    };

    BinaryReader _gff;

    uint32_t _structOffset {0};
    int _structCount {0};
    uint32_t _fieldOffset {0};
    int _fieldCount {0};
    uint32_t _labelOffset {0};
    int _labelCount {0};
    uint32_t _fieldDataOffset {0};
    int _fieldDataCount {0};
    uint32_t _fieldIndicesOffset {0};
    int _fieldIncidesCount {0};
    uint32_t _listIndicesOffset {0};
    int _listIndicesCount {0};
    std::shared_ptr<Gff> _root;

    std::unique_ptr<Gff> readStruct(int idx);
    Gff::Field readField(int idx);
    std::string readLabel(int idx);
    std::vector<uint32_t> readFieldIndices(uint32_t off, int count);
    uint64_t readQWordFieldData(uint32_t off);
    std::string readStringFieldData(uint32_t off);
    std::string readResRefFieldData(uint32_t off);
    LocString readCExoLocStringFieldData(uint32_t off);
    int32_t readStrRefFieldData(uint32_t off);
    ByteArray readByteArrayFieldData(uint32_t off);
    ByteArray readByteArrayFieldData(uint32_t off, int size);
    std::vector<uint32_t> readList(uint32_t off);
};

} // namespace resource

} // namespace reone
