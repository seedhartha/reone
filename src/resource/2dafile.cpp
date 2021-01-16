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

#include "2dafile.h"

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "../common/log.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static constexpr int kSignatureSize = 8;
static constexpr char kSignature[] = "2DA V2.b";

void TwoDaRow::add(const string &column, const string &value) {
    _values.insert(make_pair(column, value));
}

const string &TwoDaRow::getString(const string &column) const {
    auto val = _values.find(column);
    return val->second;
}

int TwoDaRow::getInt(const string &column) const {
    string value(getString(column));
    return !value.empty() ? stoi(value) : -1;
}

float TwoDaRow::getFloat(const string &column) const {
    return stof(getString(column));
}

const unordered_map<string, string> &TwoDaRow::values() const {
    return _values;
}

const TwoDaRow *TwoDaTable::findRow(const function<bool(const TwoDaRow &)> &pred) const {
    auto row = find_if(_rows.begin(), _rows.end(), pred);
    return row != _rows.end() ? &*row : nullptr;
}

const TwoDaRow *TwoDaTable::findRowByColumnValue(const string &columnName, const string &columnValue) const {
    if (find(_headers.begin(), _headers.end(), columnName) == _headers.end()) {
        throw logic_error("2DA: column not found: " + columnName);
    }
    for (auto &row : _rows) {
        if (row.getString(columnName) == columnValue) {
            return &row;
        }
    }
    warn(boost::format("2DA: cell not found: %s %s") % columnName % columnValue);

    return nullptr;
}

const string &TwoDaTable::getString(int row, const string &column) const {
    if (row < 0 || row >= _rows.size()) {
        throw out_of_range("2DA: row index out of range: " + to_string(row));
    }
    if (find(_headers.begin(), _headers.end(), column) == _headers.end()) {
        throw logic_error("2DA: column not found: " + column);
    }

    return _rows[row].getString(column);
}

int TwoDaTable::getInt(int row, const string &column, int defValue) const {
    const string &value = getString(row, column);
    if (value.empty()) return defValue;

    return stoi(value);
}

uint32_t TwoDaTable::getUint(int row, const string &column, uint32_t defValue) const {
    const string &value = getString(row, column);
    if (value.empty()) return defValue;

    return stoi(value, nullptr, 16);
}

float TwoDaTable::getFloat(int row, const string &column, float defValue) const {
    const string &value = getString(row, column);
    if (value.empty()) return defValue;

    return stof(value);
}

const vector<string> &TwoDaTable::headers() const {
    return _headers;
}

const vector<TwoDaRow> &TwoDaTable::rows() const {
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
    string token;
    while (readToken(token)) {
        _table->_headers.push_back(token);
    }
}

bool TwoDaFile::readToken(string &token) {
    size_t pos = tell();

    char buf[256];
    streamsize chRead = _in->rdbuf()->sgetn(buf, sizeof(buf));
    const char *pch = buf;

    for (; pch - buf < chRead; ++pch) {
        if (*pch == '\0') {
            seek(pos + pch - buf + 1);
            return false;
        }
        if (*pch == '\t') {
            string s(buf, pch - buf);
            seek(pos + pch - buf + 1);
            token = move(s);
            return true;
        }
    }

    throw runtime_error("2DA token not terminated");
}

void TwoDaFile::loadLabels() {
    string token;
    for (int i = 0; i < _rowCount; ++i) {
        readToken(token);
    }
}

void TwoDaFile::loadRows() {
    _table->_rows.reserve(_rowCount);

    int columnCount = static_cast<int>(_table->_headers.size());
    int cellCount = _rowCount * columnCount;
    vector<uint16_t> offsets(cellCount);

    for (int i = 0; i < cellCount; ++i) {
        offsets[i] = readUint16();
    }

    uint16_t dataSize = readUint16();
    size_t pos = tell();

    for (int i = 0; i < _rowCount; ++i) {
        TwoDaRow row;

        for (int j = 0; j < columnCount; ++j) {
            const string &name = _table->_headers[j];
            int cellIdx = i * columnCount + j;
            size_t off = pos + offsets[cellIdx];

            row.add(name, readCStringAt(off));
        }

        _table->_rows.push_back(row);
    }
}

const shared_ptr<TwoDaTable> &TwoDaFile::table() const {
    return _table;
}

} // namespace bioware

} // namespace reone
