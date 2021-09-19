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

#include "gffreader.h"

#include "../../common/logutil.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static constexpr int kSignatureSize = 8;

GffReader::GffReader() : BinaryReader(kSignatureSize) {
}

void GffReader::doLoad() {
    _structOffset = readUint32();
    _structCount = readUint32();
    _fieldOffset = readUint32();
    _fieldCount = readUint32();
    _labelOffset = readUint32();
    _labelCount = readUint32();
    _fieldDataOffset = readUint32();
    _fieldDataCount = readUint32();
    _fieldIndicesOffset = readUint32();
    _fieldIncidesCount = readUint32();
    _listIndicesOffset = readUint32();
    _listIndicesCount = readUint32();
    _root = move(readStruct(0));
}

unique_ptr<GffStruct> GffReader::readStruct(int idx) {
    seek(_structOffset + 12ll * idx);

    uint32_t type = readUint32();
    uint32_t dataOffset = readUint32();
    uint32_t fieldCount = readUint32();

    auto gffs = make_unique<GffStruct>(type);

    if (fieldCount == 1) {
        gffs->_fields.push_back(readField(dataOffset));
    } else {
        vector<uint32_t> indices(readFieldIndices(dataOffset, fieldCount));
        for (auto &idx : indices) {
            gffs->_fields.push_back(readField(idx));
        }
    }

    return move(gffs);
}

GffField GffReader::readField(int idx) {
    seek(_fieldOffset + 12ll * idx);

    uint32_t type = readUint32();
    uint32_t labelIndex = readUint32();
    uint32_t dataOrDataOffset = readUint32();

    GffField field;
    field.type = static_cast<GffFieldType>(type);
    field.label = readLabel(labelIndex);

    switch (field.type) {
        case GffFieldType::Byte:
        case GffFieldType::Word:
        case GffFieldType::Dword:
            field.uintValue = dataOrDataOffset;
            break;
        case GffFieldType::Char:
        case GffFieldType::Short:
        case GffFieldType::Int:
            field.intValue = *reinterpret_cast<int *>(&dataOrDataOffset);
            break;
        case GffFieldType::Dword64:
            field.uint64Value = readQWordFieldData(dataOrDataOffset);
            break;
        case GffFieldType::Int64: {
            uint64_t tmp = readQWordFieldData(dataOrDataOffset);
            field.int64Value = *reinterpret_cast<int64_t *>(&tmp);
            break;
        }
        case GffFieldType::Float:
            field.floatValue = *reinterpret_cast<float *>(&dataOrDataOffset);
            break;
        case GffFieldType::Double: {
            uint64_t tmp = readQWordFieldData(dataOrDataOffset);
            field.doubleValue = *reinterpret_cast<double *>(&tmp);
            break;
        }
        case GffFieldType::CExoString:
            field.strValue = readStringFieldData(dataOrDataOffset);
            break;
        case GffFieldType::ResRef:
            field.strValue = readResRefFieldData(dataOrDataOffset);
            break;
        case GffFieldType::CExoLocString: {
            LocString locString(readCExoLocStringFieldData(dataOrDataOffset));
            field.intValue = locString.strRef;
            field.strValue = locString.subString;
            break;
        }
        case GffFieldType::Void:
            field.data = readByteArrayFieldData(dataOrDataOffset);
            break;
        case GffFieldType::Struct:
            field.children.push_back(readStruct(dataOrDataOffset));
            break;
        case GffFieldType::List: {
            vector<uint32_t> list(readList(dataOrDataOffset));
            for (auto &item : list) {
                field.children.push_back(readStruct(item));
            }
            break;
        }
        case GffFieldType::Orientation: {
            ByteArray data(readByteArrayFieldData(dataOrDataOffset, 4 * sizeof(float)));
            auto floatData = reinterpret_cast<float *>(&data[0]);
            field.quatValue = glm::quat(floatData[0], floatData[1], floatData[2], floatData[3]);
            break;
        }
        case GffFieldType::Vector: {
            ByteArray data(readByteArrayFieldData(dataOrDataOffset, 3 * sizeof(float)));
            field.vecValue = glm::make_vec3(reinterpret_cast<float *>(&data[0]));
            break;
        }
        case GffFieldType::StrRef:
            field.intValue = readStrRefFieldData(dataOrDataOffset);
            break;
        default:
            throw runtime_error("Unsupported field type: " + to_string(type));
    }

    return move(field);
}

string GffReader::readLabel(int idx) {
    uint32_t off = _labelOffset + 16 * idx;
    return readCString(off, 16);
}

vector<uint32_t> GffReader::readFieldIndices(uint32_t off, int count) {
    return readUint32Array(_fieldIndicesOffset + off, count);
}

uint64_t GffReader::readQWordFieldData(uint32_t off) {
    size_t pos = tell();
    seek(_fieldDataOffset + off);
    uint64_t val = readUint64();
    seek(pos);

    return val;
}

string GffReader::readStringFieldData(uint32_t off) {
    size_t pos = tell();
    seek(_fieldDataOffset + off);

    uint32_t size = readUint32();
    string s(readCString(size));
    seek(pos);

    return move(s);
}

string GffReader::readResRefFieldData(uint32_t off) {
    size_t pos = tell();
    seek(_fieldDataOffset + off);

    uint8_t size = readByte();
    string s(readCString(size));
    seek(pos);

    return move(s);
}

GffReader::LocString GffReader::readCExoLocStringFieldData(uint32_t off) {
    size_t pos = tell();
    seek(_fieldDataOffset + off);

    uint32_t size = readUint32();
    int32_t ref = readInt32();
    uint32_t count = readUint32();

    LocString loc;
    loc.strRef = ref;

    if (count > 0) {
        int32_t type = readInt32();
        uint32_t ssSize = readUint32();
        loc.subString = readCString(ssSize);

        if (count > 1) {
            warn("GFF: more than one substring in CExoLocString, ignoring");
        }
    }

    seek(pos);

    return move(loc);
}

int32_t GffReader::readStrRefFieldData(uint32_t off) {
    size_t pos = tell();
    seek(_fieldDataOffset + off);

    uint32_t size = readUint32();
    int32_t ref = readInt32();

    seek(pos);

    return ref;
}

ByteArray GffReader::readByteArrayFieldData(uint32_t off) {
    size_t pos = tell();
    seek(_fieldDataOffset + off);

    uint32_t size = readUint32();
    ByteArray arr(readBytes(size));
    seek(pos);

    return move(arr);
}

ByteArray GffReader::readByteArrayFieldData(uint32_t off, int size) {
    return readBytes(_fieldDataOffset + off, size);
}

vector<uint32_t> GffReader::readList(uint32_t off) {
    size_t pos = tell();
    seek(static_cast<size_t>(_listIndicesOffset) + off);

    uint32_t count = readUint32();
    vector<uint32_t> arr(readUint32Array(count));
    seek(pos);

    return move(arr);
}

} // namespace resource

} // namespace reone
