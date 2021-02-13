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

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "../../common/log.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static constexpr int kSignatureSize = 8;
static const char kSignature[] = "2DA V2.b";

void TwoDaRow::add(const string &column, const string &value) {
    _values.push_back(make_pair(column, value));
}

const string &TwoDaRow::getString(const string &column) const {
    static string empty;

    auto maybeValue = find_if(_values.begin(), _values.end(), [this, &column](auto &pair) {
        return pair.first == column && pair.second != "****";
    });
    return maybeValue != _values.end() ? maybeValue->second : empty;
}

int TwoDaRow::getInt(const string &column, int defValue) const {
    const string &value = getString(column);
    return !value.empty() ? stoi(value) : defValue;
}

float TwoDaRow::getFloat(const string &column, float defValue) const {
    const string &value = getString(column);
    return !value.empty() ? stof(value) : defValue;
}

bool TwoDaRow::getBool(const string &column, bool defValue) const {
    const string &value = getString(column);
    return !value.empty() ? (stoi(value) != 0) : defValue;
}

void TwoDaTable::add(TwoDaRow row) {
    // If headers are not initialized, initialize them from this row
    if (_headers.empty()) {
        for (auto &column : row.values()) {
            _headers.push_back(column.first);
        }
    }
    _rows.push_back(move(row));
}

const TwoDaRow *TwoDaTable::get(const function<bool(const TwoDaRow &)> &pred) const {
    auto row = find_if(_rows.begin(), _rows.end(), pred);
    return row != _rows.end() ? &*row : nullptr;
}

const TwoDaRow *TwoDaTable::getByColumnValue(const string &columnName, const string &columnValue) const {
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
    static string empty;

    if (row < 0 || row >= _rows.size()) {
        error("2DA: row index out of range: " + to_string(row));
        return empty;
    }
    if (find(_headers.begin(), _headers.end(), column) == _headers.end()) {
        error("2DA: column not found: " + column);
        return empty;
    }

    const string &value = _rows[row].getString(column);
    if (value == "****") return empty;

    return value;
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

bool TwoDaTable::getBool(int row, const string &column, bool defValue) const {
    const string &value = getString(row, column);
    if (value.empty()) return defValue;

    return stoi(value) != 0;
}

TwoDaFile::TwoDaFile() : BinaryFile(kSignatureSize, kSignature), _table(make_shared<TwoDaTable>()) {
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

TwoDaWriter::TwoDaWriter(const shared_ptr<TwoDaTable> &table) : _table(table) {
    if (!table) {
        throw invalid_argument("table must not be null");
    }
}

void TwoDaWriter::save(const fs::path &path) {
    auto stream = make_shared<fs::ofstream>(path, ios::binary);

    StreamWriter writer(stream);
    writer.putString(kSignature);
    writer.putChar('\n');

    writeHeaders(writer);

    size_t rowCount = _table->rows().size();
    writer.putUint32(rowCount);

    writeLabels(writer);
    writeData(writer);
}

void TwoDaWriter::writeHeaders(StreamWriter &writer) {
    for (auto &header : _table->headers()) {
        writer.putString(header);
        writer.putChar('\t');
    }
    writer.putChar('\0');
}

void TwoDaWriter::writeLabels(StreamWriter &writer) {
    for (size_t i = 0; i < _table->rows().size(); ++i) {
        writer.putString(to_string(i));
        writer.putChar('\t');
    }
}

void TwoDaWriter::writeData(StreamWriter &writer) {
    vector<pair<string, int>> data;
    int dataSize = 0;

    size_t columnCount = _table->headers().size();
    size_t cellCount = columnCount * _table->rows().size();

    for (size_t i = 0; i < _table->rows().size(); ++i) {
        for (size_t j = 0; j < columnCount; ++j) {
            string value(_table->rows()[i].values()[j].second);
            auto maybeData = find_if(data.begin(), data.end(), [&](auto &pair) { return pair.first == value; });
            if (maybeData != data.end()) {
                writer.putUint16(maybeData->second);
            } else {
                data.push_back(make_pair(value, dataSize));
                writer.putUint16(dataSize);
                int len = strnlen(&value[0], value.length());
                dataSize += len + 1ll;
            }
        }
    }

    writer.putUint16(dataSize);

    for (auto &pair : data) {
        writer.putCString(pair.first);
    }
}

} // namespace bioware

} // namespace reone
