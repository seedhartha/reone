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

#include "2da.h"

#include "../common/collectionutil.h"
#include "../common/logutil.h"

using namespace std;

namespace reone {

namespace resource {

static constexpr char kCellValueDeleted[] = "****";

void TwoDA::addColumn(string name) {
    _columns.push_back(move(name));
}

void TwoDA::add(Row row) {
    _rows.push_back(move(row));
}

int TwoDA::indexByCellValue(const string &column, const string &value) const {
    int columnIdx = getColumnIndex(column);
    if (columnIdx == -1) {
        warn("2DA: column not found: " + column);
        return -1;
    }
    for (size_t i = 0; i < _rows.size(); ++i) {
        if (_rows[i].values[columnIdx] == value) return static_cast<int>(i);
    }

    return -1;
}

int TwoDA::getColumnIndex(const string &column) const {
    for (size_t i = 0; i < _columns.size(); ++i) {
        if (_columns[i] == column) return static_cast<int>(i);
    }
    return -1;
}

static vector<string> getColumnNames(const vector<pair<string, string>> &values) {
    return transform<pair<string, string>, string>(values, [](auto &pair) { return pair.first; });
}

int TwoDA::indexByCellValues(const vector<pair<string, string>> &values) const {
    vector<string> columns(getColumnNames(values));
    vector<int> columnIndices(getColumnIndices(columns));

    for (size_t i = 0; i < _rows.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < values.size(); ++j) {
            int columnIdx = columnIndices[j];
            if (_rows[i].values[columnIdx] != values[j].second) {
                match = false;
                break;
            }
        }
        if (match) return static_cast<int>(i);
    }

    return -1;
}

vector<int> TwoDA::getColumnIndices(const vector<string> &columns) const {
    vector<int> indices;
    for (auto &column : columns) {
        int index = getColumnIndex(column);
        if (index == -1) {
            throw logic_error("Column not found: " + column);
        }
        indices.push_back(index);
    }
    return move(indices);
}

string TwoDA::getString(int row, const string &column, string defValue) const {
    if (row < 0 || row >= _rows.size()) {
        warn("2DA: row index out of range: " + to_string(row));
        return move(defValue);
    }

    int columnIdx = getColumnIndex(column);
    if (columnIdx == -1) {
        warn("2DA: column not found: " + column);
        return move(defValue);
    }

    const string &value = _rows[row].values[columnIdx];

    if (value == kCellValueDeleted) {
        warn(boost::format("2DA: cell value was deleted: %d %s") % row % column);
        return move(defValue);
    }

    return value;
}

int TwoDA::getInt(int row, const string &column, int defValue) const {
    const string &value = getString(row, column);
    if (value.empty()) return defValue;

    return stoi(value);
}

uint32_t TwoDA::getUint(int row, const string &column, uint32_t defValue) const {
    const string &value = getString(row, column);
    if (value.empty()) return defValue;

    return stoi(value, nullptr, 16);
}

float TwoDA::getFloat(int row, const string &column, float defValue) const {
    const string &value = getString(row, column);
    if (value.empty()) return defValue;

    return stof(value);
}

bool TwoDA::getBool(int row, const string &column, bool defValue) const {
    const string &value = getString(row, column);
    if (value.empty()) return defValue;

    return stoi(value) != 0;
}

} // namespace resource

} // namespace reone
