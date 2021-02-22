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

#include "gffwriter.h"

#include <stdexcept>
#include <unordered_map>

#include <boost/filesystem/fstream.hpp>

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static const unordered_map<ResourceType, string> g_signatures {
    { ResourceType::Are, "ARE" },
    { ResourceType::Dlg, "DLG" },
    { ResourceType::Git, "GIT" },
    { ResourceType::Gui, "GUI" },
    { ResourceType::Ifo, "IFO" },
    { ResourceType::Jrl, "JRL" },
    { ResourceType::Utc, "UTC" },
    { ResourceType::Utd, "UTD" },
    { ResourceType::Ute, "UTE" },
    { ResourceType::Uti, "UTI" },
    { ResourceType::Utm, "UTM" },
    { ResourceType::Utp, "UTP" },
    { ResourceType::Uts, "UTS" },
    { ResourceType::Utt, "UTT" },
    { ResourceType::Utw, "UTW" }
};

GffWriter::GffWriter(ResourceType resType, const shared_ptr<GffStruct> &root) : _resType(resType), _root(root) {
    if (!root) {
        throw invalid_argument("root must not be null");
    }
}

void GffWriter::save(const fs::path &path) {
    auto out = make_shared<fs::ofstream>(path, ios::binary);
    save(out);
}

void GffWriter::save(const shared_ptr<ostream> &out) {
    _context.structs.resize(1);

    visit(*_root, 0);

    _writer = make_unique<StreamWriter>(out);

    writeHeader();
    writeStructArray();
    writeFieldArray();
    writeLabelArray();
    writeFieldData();
    writeFieldIndices();
    writeListIndices();
}

void GffWriter::visit(const GffStruct &gffs, int index) {
    vector<uint32_t> fieldIndices;
    vector<pair<GffStruct *, int>> toVisit;

    for (auto &field : gffs.fields()) {
        fieldIndices.push_back(_context.fields.size());

        // Save field label
        int labelIdx;
        auto maybeLabel = find(_context.labels.begin(), _context.labels.end(), field->label);
        if (maybeLabel != _context.labels.end()) {
            labelIdx = distance(_context.labels.begin(), maybeLabel);
        } else {
            labelIdx = static_cast<int>(_context.labels.size());
            _context.labels.push_back(field->label);
        }

        // Save field data
        uint32_t dataOrDataOffset;
        size_t numStructs = _context.structs.size();
        size_t sizeListIndices = _context.listIndices.size();
        size_t sizeFieldData = _context.fieldData.size();
        ByteArray fieldData;
        switch (field->type) {
            case GffStruct::FieldType::Byte:
            case GffStruct::FieldType::Word:
            case GffStruct::FieldType::Dword:
                dataOrDataOffset = field->uintValue;
                break;
            case GffStruct::FieldType::Char:
            case GffStruct::FieldType::Short:
            case GffStruct::FieldType::Int:
                dataOrDataOffset = *reinterpret_cast<uint32_t *>(&field->intValue);
                break;
            case GffStruct::FieldType::Dword64:
                fieldData.resize(8);
                memcpy(&fieldData[0], &field->uint64Value, 8);
                dataOrDataOffset = sizeFieldData;
                break;
            case GffStruct::FieldType::Int64:
                fieldData.resize(8);
                memcpy(&fieldData[0], &field->int64Value, 8);
                dataOrDataOffset = sizeFieldData;
                break;
            case GffStruct::FieldType::Float:
                dataOrDataOffset = *reinterpret_cast<uint32_t *>(&field->floatValue);
                break;
            case GffStruct::FieldType::Double:
                fieldData.resize(8);
                memcpy(&fieldData[0], &field->doubleValue, sizeof(double));
                dataOrDataOffset = sizeFieldData;
                break;
            case GffStruct::FieldType::CExoString: {
                uint32_t length = field->strValue.length();
                fieldData.resize(4ll + length);
                memcpy(&fieldData[0], &length, 4);
                memcpy(&fieldData[4], &field->strValue[0], length);
                dataOrDataOffset = sizeFieldData;
                break;
            }
            case GffStruct::FieldType::ResRef: {
                uint32_t length = field->strValue.length();
                fieldData.resize(1ll + length);
                fieldData[0] = length;
                memcpy(&fieldData[1], &field->strValue[0], length);
                dataOrDataOffset = sizeFieldData;
                break;
            }
            case GffStruct::FieldType::CExoLocString: {
                uint32_t numSubstrings = !field->strValue.empty() ? 1 : 0;
                uint32_t totalSize = 8 + (numSubstrings > 0 ? (8 + field->strValue.length()) : 0);
                fieldData.resize(4ll + totalSize);
                memcpy(&fieldData[0], &totalSize, 4);
                memcpy(&fieldData[4], &field->intValue, 4);
                memcpy(&fieldData[8], &numSubstrings, 4);
                if (numSubstrings > 0) {
                    uint32_t id = 0;
                    uint32_t length = field->strValue.length();
                    memcpy(&fieldData[12], &id, 4);
                    memcpy(&fieldData[16], &length, 4);
                    memcpy(&fieldData[20], &field->strValue[0], length);
                }
                dataOrDataOffset = sizeFieldData;
                break;
            }
            case GffStruct::FieldType::Void: {
                uint32_t dataSize = field->data.size();
                fieldData.resize(4ll + dataSize);
                memcpy(&fieldData[0], &dataSize, 4);
                memcpy(&fieldData[4], &field->data[0], dataSize);
                dataOrDataOffset = sizeFieldData;
                break;
            }
            case GffStruct::FieldType::Struct:
                dataOrDataOffset = numStructs;
                _context.structs.resize(numStructs + 1);
                toVisit.push_back(make_pair(field->children[0].get(), numStructs));
                break;
            case GffStruct::FieldType::List: {
                uint32_t numChildren = field->children.size();
                _context.structs.resize(numStructs + numChildren);
                _context.listIndices.push_back(numChildren);
                for (size_t i = 0; i < field->children.size(); ++i) {
                    _context.listIndices.push_back(numStructs + i);
                    toVisit.push_back(make_pair(field->children[i].get(), numStructs + i));
                }
                dataOrDataOffset = sizeListIndices / 4;
                break;
            }
            case GffStruct::FieldType::Orientation:
                fieldData.resize(16);
                memcpy(&fieldData[0], &field->quatValue[0], 16);
                dataOrDataOffset = sizeFieldData;
                break;
            case GffStruct::FieldType::Vector:
                fieldData.resize(12);
                memcpy(&fieldData[0], &field->vecValue[0], 12);
                dataOrDataOffset = sizeFieldData;
                break;
            case GffStruct::FieldType::StrRef: {
                uint32_t totalSize = 4;
                fieldData.resize(8);
                memcpy(&fieldData[0], &totalSize, 4);
                memcpy(&fieldData[4], &field->intValue, 4);
                dataOrDataOffset = sizeFieldData;
                break;
            }
            default:
                throw logic_error("Unsupported field type: " + static_cast<int>(field->type));
        }
        if (!fieldData.empty()) {
            copy(fieldData.begin(), fieldData.end(), back_inserter(_context.fieldData));
        }

        // Save field
        WriteField writeField;
        writeField.type = static_cast<uint32_t>(field->type);
        writeField.labelIndex = labelIdx;
        writeField.dataOrDataOffset = dataOrDataOffset;
        _context.fields.push_back(move(writeField));
    }

    uint32_t dataOrDataOffset;
    if (fieldIndices.size() == 1ll) {
        dataOrDataOffset = fieldIndices[0];
    } else {
        dataOrDataOffset = _context.fieldIndices.size() * sizeof(uint32_t);
        copy(fieldIndices.begin(), fieldIndices.end(), back_inserter(_context.fieldIndices));
    }

    _context.structs[index].type = (index == 0) ? 0xffffffff : 0;
    _context.structs[index].dataOrDataOffset = dataOrDataOffset;
    _context.structs[index].fieldCount = gffs.fields().size();

    for (auto &pair : toVisit) {
        visit(*pair.first, pair.second);
    }
}

void GffWriter::writeHeader() {
    auto maybeSignature = g_signatures.find(_resType);
    if (maybeSignature == g_signatures.end()) {
        throw logic_error("Unsupported GFF resource type: " + to_string(static_cast<int>(_resType)));
    }

    size_t numStructs = _context.structs.size();
    size_t numFields = _context.fields.size();
    size_t numLabels = _context.labels.size();
    size_t numFieldIndices = _context.fieldIndices.size();
    size_t numListIndices = _context.listIndices.size();

    size_t sizeStructs = numStructs * 3 * sizeof(uint32_t);
    size_t sizeFields = numFields * 3 * sizeof(uint32_t);
    size_t sizeLabels = numLabels * 16;
    size_t sizeFieldData = _context.fieldData.size();
    size_t sizeFieldIndices = numFieldIndices * sizeof(uint32_t);
    size_t sizeListIndices = numListIndices * sizeof(uint32_t);

    size_t offStructs = 0x38;
    size_t offFields = offStructs + sizeStructs;
    size_t offLabels = offFields + sizeFields;
    size_t offFieldData = offLabels + sizeLabels;
    size_t offFieldIndices = offFieldData + sizeFieldData;
    size_t offListIndices = offFieldIndices + sizeFieldIndices;

    _writer->putString(maybeSignature->second);
    _writer->putString(" V3.2");
    _writer->putUint32(offStructs); // struct array offset
    _writer->putUint32(numStructs); // number of structs
    _writer->putUint32(offFields); // field array offset
    _writer->putUint32(numFields); // number of fields
    _writer->putUint32(offLabels); // label array offset
    _writer->putUint32(numLabels); // number of labels
    _writer->putUint32(offFieldData); // field data array offset
    _writer->putUint32(sizeFieldData); // number of bytes in field data
    _writer->putUint32(offFieldIndices); // field indices array offset
    _writer->putUint32(sizeFieldIndices); // number of bytes in field indices array
    _writer->putUint32(offListIndices); // list indices array offset
    _writer->putUint32(sizeListIndices); // number of bytes in list indices array
}

void GffWriter::writeStructArray() {
    for (auto &writeStruct : _context.structs) {
        _writer->putUint32(writeStruct.type);
        _writer->putUint32(writeStruct.dataOrDataOffset);
        _writer->putUint32(writeStruct.fieldCount);
    }
}

void GffWriter::writeFieldArray() {
    for (auto &field : _context.fields) {
        _writer->putUint32(field.type);
        _writer->putUint32(field.labelIndex);
        _writer->putUint32(field.dataOrDataOffset);
    }
}

void GffWriter::writeLabelArray() {
    for (auto &label : _context.labels) {
        string tmp;
        tmp.resize(16);
        strncpy(&tmp[0], label.c_str(), 16);
        _writer->putString(tmp);
    }
}

void GffWriter::writeFieldData() {
    _writer->putBytes(_context.fieldData);
}

void GffWriter::writeFieldIndices() {
    for (auto &index : _context.fieldIndices) {
        _writer->putUint32(index);
    }
}

void GffWriter::writeListIndices() {
    for (auto &index : _context.listIndices) {
        _writer->putUint32(index);
    }
}

} // namespace resource

} // namespace reone
