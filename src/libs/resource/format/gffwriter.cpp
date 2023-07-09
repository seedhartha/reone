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

#include "reone/resource/format/gffwriter.h"

#include "reone/resource/exception/format.h"
#include "reone/system/stream/fileoutput.h"

#include "reone/resource/gff.h"

namespace reone {

namespace resource {

enum class FieldClassification {
    Simple,
    Complex,
    Struct,
    List
};

static const std::unordered_map<ResourceType, std::string> g_signatures {
    {ResourceType::Res, "RES"},
    {ResourceType::Are, "ARE"},
    {ResourceType::Dlg, "DLG"},
    {ResourceType::Git, "GIT"},
    {ResourceType::Gui, "GUI"},
    {ResourceType::Ifo, "IFO"},
    {ResourceType::Jrl, "JRL"},
    {ResourceType::Utc, "UTC"},
    {ResourceType::Utd, "UTD"},
    {ResourceType::Ute, "UTE"},
    {ResourceType::Uti, "UTI"},
    {ResourceType::Utm, "UTM"},
    {ResourceType::Utp, "UTP"},
    {ResourceType::Uts, "UTS"},
    {ResourceType::Utt, "UTT"},
    {ResourceType::Utw, "UTW"},
    {ResourceType::Pth, "PTH"}};

void GffWriter::save(const boost::filesystem::path &path) {
    auto out = FileOutputStream(path);
    save(out);
}

void GffWriter::save(IOutputStream &out) {
    processTree();

    _writer = std::make_unique<BinaryWriter>(out);

    writeHeader();
    writeStructArray();
    writeFieldArray();
    writeLabelArray();
    writeFieldData();
    writeFieldIndices();
    writeListIndices();
}

static FieldClassification getFieldData(const Gff::Field &field, uint32_t &simple, ByteArray &complex) {
    switch (field.type) {
    case Gff::FieldType::Byte:
    case Gff::FieldType::Word:
    case Gff::FieldType::Dword:
        simple = field.uintValue;
        return FieldClassification::Simple;

    case Gff::FieldType::Char:
    case Gff::FieldType::Short:
    case Gff::FieldType::Int:
        simple = *reinterpret_cast<const uint32_t *>(&field.intValue);
        return FieldClassification::Simple;

    case Gff::FieldType::Dword64:
        complex.resize(8);
        memcpy(&complex[0], &field.uint64Value, 8);
        return FieldClassification::Complex;

    case Gff::FieldType::Int64:
        complex.resize(8);
        memcpy(&complex[0], &field.int64Value, 8);
        return FieldClassification::Complex;

    case Gff::FieldType::Float:
        simple = *reinterpret_cast<const uint32_t *>(&field.floatValue);
        return FieldClassification::Simple;

    case Gff::FieldType::Double:
        complex.resize(8);
        memcpy(&complex[0], &field.doubleValue, sizeof(double));
        return FieldClassification::Complex;

    case Gff::FieldType::CExoString: {
        uint32_t length = static_cast<uint32_t>(field.strValue.length());
        complex.resize(4ll + length);
        memcpy(&complex[0], &length, 4);
        memcpy(&complex[4], &field.strValue[0], length);
        return FieldClassification::Complex;
    }
    case Gff::FieldType::ResRef: {
        uint32_t length = static_cast<uint32_t>(field.strValue.length());
        complex.resize(1ll + length);
        complex[0] = length;
        memcpy(&complex[1], &field.strValue[0], length);
        return FieldClassification::Complex;
    }
    case Gff::FieldType::CExoLocString: {
        uint32_t numSubstrings = !field.strValue.empty() ? 1 : 0;
        uint32_t totalSize = static_cast<uint32_t>(8 + (numSubstrings > 0 ? (8 + field.strValue.length()) : 0));
        complex.resize(4ll + totalSize);
        memcpy(&complex[0], &totalSize, 4);
        memcpy(&complex[4], &field.intValue, 4);
        memcpy(&complex[8], &numSubstrings, 4);
        if (numSubstrings > 0) {
            uint32_t id = 0;
            uint32_t length = static_cast<uint32_t>(field.strValue.length());
            memcpy(&complex[12], &id, 4);
            memcpy(&complex[16], &length, 4);
            memcpy(&complex[20], &field.strValue[0], length);
        }
        return FieldClassification::Complex;
    }
    case Gff::FieldType::Void: {
        uint32_t dataSize = static_cast<uint32_t>(field.data.size());
        complex.resize(4ll + dataSize);
        memcpy(&complex[0], &dataSize, 4);
        memcpy(&complex[4], &field.data[0], dataSize);
        return FieldClassification::Complex;
    }
    case Gff::FieldType::Struct:
        return FieldClassification::Struct;

    case Gff::FieldType::List:
        return FieldClassification::List;

    case Gff::FieldType::Orientation:
        complex.resize(16);
        memcpy(&complex[0], &field.quatValue.w, 4);
        memcpy(&complex[4], &field.quatValue.x, 4);
        memcpy(&complex[8], &field.quatValue.y, 4);
        memcpy(&complex[12], &field.quatValue.z, 4);
        return FieldClassification::Complex;

    case Gff::FieldType::Vector:
        complex.resize(12);
        memcpy(&complex[0], &field.vecValue[0], 12);
        return FieldClassification::Complex;

    case Gff::FieldType::StrRef: {
        uint32_t totalSize = 4;
        complex.resize(8);
        memcpy(&complex[0], &totalSize, 4);
        memcpy(&complex[4], &field.intValue, 4);
        return FieldClassification::Complex;
    }
    default:
        throw FormatException("Unsupported field type: " + std::to_string(static_cast<int>(field.type)));
    }
}

void GffWriter::processTree() {
    std::queue<const Gff *> aQueue;
    aQueue.push(_root.get());

    int structIdx = 0;
    int numStructs = 0;

    while (!aQueue.empty()) {
        const Gff &aStruct = *aQueue.front();
        aQueue.pop();

        std::vector<uint32_t> fieldIndices;

        for (auto &field : aStruct.fields()) {
            // Current number of fields is a new field index
            fieldIndices.push_back(static_cast<uint32_t>(_context.fields.size()));

            // Append or use existing field label
            int labelIdx;
            auto maybeLabel = find(_context.labels.begin(), _context.labels.end(), field.label);
            if (maybeLabel != _context.labels.end()) {
                labelIdx = static_cast<int>(distance(_context.labels.begin(), maybeLabel));
            } else {
                labelIdx = static_cast<int>(_context.labels.size());
                _context.labels.push_back(field.label);
            }

            // Retrieve and save field data
            uint32_t dataOrDataOffset;
            ByteArray complexData;
            FieldClassification fieldClass = getFieldData(field, dataOrDataOffset, complexData);
            switch (fieldClass) {
            case FieldClassification::Complex:
                dataOrDataOffset = static_cast<uint32_t>(_context.fieldData.size());
                copy(complexData.begin(), complexData.end(), back_inserter(_context.fieldData));
                break;
            case FieldClassification::Struct:
                // Set data offset to the next struct index
                dataOrDataOffset = ++numStructs;
                aQueue.push(field.children[0].get());
                break;
            case FieldClassification::List:
                // Set data offset to the current size of the list indices array
                dataOrDataOffset = static_cast<uint32_t>(4 * _context.listIndices.size());
                _context.listIndices.push_back(static_cast<uint32_t>(field.children.size()));
                for (size_t i = 0; i < field.children.size(); ++i) {
                    _context.listIndices.push_back(++numStructs);
                    aQueue.push(field.children[i].get());
                }
                break;
            default:
                break;
            }

            // Save field
            WriteField writeField;
            writeField.type = static_cast<uint32_t>(field.type);
            writeField.labelIndex = labelIdx;
            writeField.dataOrDataOffset = dataOrDataOffset;
            _context.fields.push_back(std::move(writeField));
        }

        uint32_t dataOrDataOffset;
        if (fieldIndices.size() == 1ll) {
            dataOrDataOffset = fieldIndices[0];
        } else {
            dataOrDataOffset = static_cast<uint32_t>(4 * _context.fieldIndices.size());
            copy(fieldIndices.begin(), fieldIndices.end(), back_inserter(_context.fieldIndices));
        }

        WriteStruct writeStruct;
        writeStruct.type = aStruct.type();
        writeStruct.dataOrDataOffset = dataOrDataOffset;
        writeStruct.fieldCount = static_cast<uint32_t>(aStruct.fields().size());
        _context.structs.push_back(std::move(writeStruct));
    }
}

void GffWriter::writeHeader() {
    auto maybeSignature = g_signatures.find(_resType);
    if (maybeSignature == g_signatures.end()) {
        throw std::logic_error("Unsupported GFF resource type: " + std::to_string(static_cast<int>(_resType)));
    }

    uint32_t numStructs = static_cast<int>(_context.structs.size());
    uint32_t numFields = static_cast<int>(_context.fields.size());
    uint32_t numLabels = static_cast<int>(_context.labels.size());
    uint32_t numFieldIndices = static_cast<int>(_context.fieldIndices.size());
    uint32_t numListIndices = static_cast<int>(_context.listIndices.size());

    uint32_t sizeStructs = numStructs * 3 * sizeof(uint32_t);
    uint32_t sizeFields = numFields * 3 * sizeof(uint32_t);
    uint32_t sizeLabels = numLabels * 16;
    uint32_t sizeFieldData = static_cast<int>(_context.fieldData.size());
    uint32_t sizeFieldIndices = numFieldIndices * sizeof(uint32_t);
    uint32_t sizeListIndices = numListIndices * sizeof(uint32_t);

    uint32_t offStructs = 0x38;
    uint32_t offFields = offStructs + sizeStructs;
    uint32_t offLabels = offFields + sizeFields;
    uint32_t offFieldData = offLabels + sizeLabels;
    uint32_t offFieldIndices = offFieldData + sizeFieldData;
    uint32_t offListIndices = offFieldIndices + sizeFieldIndices;

    _writer->writeString(maybeSignature->second);
    _writer->writeString(" V3.2");
    _writer->writeUint32(offStructs);       // struct array offset
    _writer->writeUint32(numStructs);       // number of structs
    _writer->writeUint32(offFields);        // field array offset
    _writer->writeUint32(numFields);        // number of fields
    _writer->writeUint32(offLabels);        // label array offset
    _writer->writeUint32(numLabels);        // number of labels
    _writer->writeUint32(offFieldData);     // field data array offset
    _writer->writeUint32(sizeFieldData);    // number of bytes in field data
    _writer->writeUint32(offFieldIndices);  // field indices array offset
    _writer->writeUint32(sizeFieldIndices); // number of bytes in field indices array
    _writer->writeUint32(offListIndices);   // list indices array offset
    _writer->writeUint32(sizeListIndices);  // number of bytes in list indices array
}

void GffWriter::writeStructArray() {
    for (auto &writeStruct : _context.structs) {
        _writer->writeUint32(writeStruct.type);
        _writer->writeUint32(writeStruct.dataOrDataOffset);
        _writer->writeUint32(writeStruct.fieldCount);
    }
}

void GffWriter::writeFieldArray() {
    for (auto &field : _context.fields) {
        _writer->writeUint32(field.type);
        _writer->writeUint32(field.labelIndex);
        _writer->writeUint32(field.dataOrDataOffset);
    }
}

void GffWriter::writeLabelArray() {
    for (auto &label : _context.labels) {
        std::string tmp;
        tmp.resize(16);
        strncpy(&tmp[0], label.c_str(), 16);
        _writer->writeString(tmp);
    }
}

void GffWriter::writeFieldData() {
    _writer->writeBytes(_context.fieldData);
}

void GffWriter::writeFieldIndices() {
    for (auto &index : _context.fieldIndices) {
        _writer->writeUint32(index);
    }
}

void GffWriter::writeListIndices() {
    for (auto &index : _context.listIndices) {
        _writer->writeUint32(index);
    }
}

} // namespace resource

} // namespace reone
