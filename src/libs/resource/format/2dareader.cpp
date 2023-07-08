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

#include "reone/resource/format/2dareader.h"

#include "reone/resource/2da.h"
#include "reone/resource/exception/format.h"

namespace reone {

namespace resource {

void TwoDaReader::onLoad() {
    checkSignature(std::string("2DA V2.b", 8));
    ignore(1); // newline
    loadHeaders();

    _rowCount = readUint32();

    loadLabels();
    loadRows();
    loadTable();
}

void TwoDaReader::loadHeaders() {
    std::string token;
    while (readToken(token)) {
        _columns.push_back(token);
    }
}

void TwoDaReader::loadLabels() {
    std::string token;
    for (int i = 0; i < _rowCount; ++i) {
        readToken(token);
    }
}

void TwoDaReader::loadRows() {
    _rows.reserve(_rowCount);

    int columnCount = static_cast<int>(_columns.size());
    int cellCount = _rowCount * columnCount;
    std::vector<uint16_t> offsets(cellCount);
    for (int i = 0; i < cellCount; ++i) {
        offsets[i] = readUint16();
    }

    uint16_t dataSize = readUint16();
    size_t pos = tell();

    for (int i = 0; i < _rowCount; ++i) {
        TwoDa::Row row;
        for (int j = 0; j < columnCount; ++j) {
            int cellIdx = i * columnCount + j;
            size_t off = pos + offsets[cellIdx];
            row.values.push_back(readCStringAt(off));
        }
        _rows.push_back(row);
    }
}

void TwoDaReader::loadTable() {
    _twoDa = std::make_shared<TwoDa>(_columns, _rows);
}

bool TwoDaReader::readToken(std::string &token) {
    size_t pos = tell();

    auto bytes = _reader->readBytes(256);
    auto start = &bytes[0];
    auto pch = start;

    for (; pch - start < bytes.size(); ++pch) {
        if (*pch == '\0') {
            seek(pos + pch - start + 1);
            return false;
        }
        if (*pch == '\t') {
            std::string s(start, pch - start);
            seek(pos + pch - start + 1);
            token = std::move(s);
            return true;
        }
    }

    throw FormatException("2DA token not terminated");
}

} // namespace resource

} // namespace reone
