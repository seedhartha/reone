/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "2dafile.h"

#include <iostream>

#include <boost/format.hpp>

#include "../core/log.h"

namespace fs = boost::filesystem;

namespace reone {

namespace resources {

static const int kSignatureSize = 8;
static const char kSignature[] = "2DA V2.b";

const std::string &TwoDaTable::getString(int row, const std::string &column) const {
    if (row >= _rows.size()) {
        throw std::out_of_range("2DA: row index out of range: " + std::to_string(row));
    }
    auto it = std::find(_headers.begin(), _headers.end(), column);
    if (it == _headers.end()) {
        throw std::logic_error("2DA: column not found: " + column);
    }
    int idx = std::distance(_headers.begin(), it);

    return _rows[row].values[idx];
}

int TwoDaTable::getInt(int row, const std::string &column, int defValue) const {
    const std::string &value = getString(row, column);
    if (value.empty()) return defValue;

    return std::stoi(value);
}

uint32_t TwoDaTable::getUint(int row, const std::string &column, uint32_t defValue) const {
    const std::string &value = getString(row, column);
    if (value.empty()) return defValue;

    return std::stoi(value, nullptr, 16);
}

float TwoDaTable::getFloat(int row, const std::string &column, float defValue) const {
    const std::string &value = getString(row, column);
    if (value.empty()) return defValue;

    return std::stof(value);
}

const std::string &TwoDaTable::getStringFromRowByColumnValue(const std::string &column, const std::string &rowByColumn, const std::string &columnValue, const std::string &defValue) const {
    int columnIdx = std::distance(_headers.begin(), std::find(_headers.begin(), _headers.end(), column));
    int rowByColumnIdx = std::distance(_headers.begin(), std::find(_headers.begin(), _headers.end(), rowByColumn));

    for (auto &row : _rows) {
        if (row.values[rowByColumnIdx] == columnValue) {
            return row.values[columnIdx];
        }
    }

    warn(boost::format("Cell not found: %s %s %s") % column % rowByColumn % columnValue);

    return defValue;
}

const std::vector<std::string> &TwoDaTable::headers() const {
    return _headers;
}

const std::vector<TwoDaRow> &TwoDaTable::rows() const {
    return _rows;
}

TwoDaFile::TwoDaFile() : BinaryFile(kSignatureSize, kSignature), _table(new TwoDaTable()) {
}

void TwoDaFile::doLoad() {
    ignore(1); // newline
    loadHeaders();

    _rowCount = readUint32();

    loadLabels();
    loadRows();
}

void TwoDaFile::loadHeaders() {
    std::string token;
    while (readToken(token)) {
        _table->_headers.push_back(token);
    }
}

bool TwoDaFile::readToken(std::string &token) {
    uint32_t pos = tell();

    char buf[256];
    std::streamsize chRead = _in->rdbuf()->sgetn(buf, sizeof(buf));
    const char *pch = buf;

    for (; pch - buf < chRead; ++pch) {
        if (*pch == '\0') {
            seek(pos + pch - buf + 1);
            return false;
        }
        if (*pch == '\t') {
            std::string s(buf, pch - buf);
            seek(pos + pch - buf + 1);
            token = std::move(s);
            return true;
        }
    }

    throw std::runtime_error("2DA: token not terminated");
}

void TwoDaFile::loadLabels() {
    std::string token;
    for (int i = 0; i < _rowCount; ++i) {
        readToken(token);
    }
}

void TwoDaFile::loadRows() {
    _table->_rows.reserve(_rowCount);

    int columnCount = _table->_headers.size();
    int cellCount = _rowCount * columnCount;
    std::vector<uint16_t> offsets(cellCount);

    for (int i = 0; i < cellCount; ++i) {
        offsets[i] = readUint16();
    }

    uint16_t dataSize = readUint16();
    uint32_t pos = tell();

    for (int i = 0; i < _rowCount; ++i) {
        TwoDaRow row;
        row.values.resize(columnCount);

        for (int j = 0; j < columnCount; ++j) {
            int cellIdx = i * columnCount + j;
            uint32_t off = pos + offsets[cellIdx];
            row.values[j] = readString(off);
        }

        _table->_rows.push_back(row);
    }
}

const std::shared_ptr<TwoDaTable> &TwoDaFile::table() const {
    return _table;
}

} // namespace bioware

} // namespace reone
