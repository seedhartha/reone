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

#include "reone/resource/2da.h"

#include "reone/system/logutil.h"

namespace reone {

namespace resource {

static constexpr char kCellValueDeleted[] = "****";

int TwoDa::indexByCellValue(const std::string &column, const std::string &value) const {
    int columnIdx = getColumnIndex(column);
    if (columnIdx == -1) {
        warn("2DA: column not found: " + column);
        return -1;
    }
    for (size_t i = 0; i < _rows.size(); ++i) {
        if (_rows[i].values[columnIdx] == value)
            return static_cast<int>(i);
    }

    return -1;
}

int TwoDa::getColumnIndex(const std::string &column) const {
    for (size_t i = 0; i < _columns.size(); ++i) {
        if (_columns[i] == column)
            return static_cast<int>(i);
    }
    return -1;
}

static std::vector<std::string> getColumnNames(const std::vector<std::pair<std::string, std::string>> &values) {
    std::vector<std::string> names;
    for (auto &val : values) {
        names.push_back(val.first);
    }
    return names;
}

int TwoDa::indexByCellValues(const std::vector<std::pair<std::string, std::string>> &values) const {
    std::vector<std::string> columns(getColumnNames(values));
    std::vector<int> columnIndices(getColumnIndices(columns));

    for (size_t i = 0; i < _rows.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < values.size(); ++j) {
            int columnIdx = columnIndices[j];
            if (_rows[i].values[columnIdx] != values[j].second) {
                match = false;
                break;
            }
        }
        if (match)
            return static_cast<int>(i);
    }

    return -1;
}

std::vector<int> TwoDa::getColumnIndices(const std::vector<std::string> &columns) const {
    std::vector<int> indices;
    for (auto &column : columns) {
        int index = getColumnIndex(column);
        if (index == -1) {
            throw std::logic_error("Column not found: " + column);
        }
        indices.push_back(index);
    }
    return std::move(indices);
}

std::string TwoDa::getString(int row, const std::string &column, std::string defValue) const {
    if (row < 0 || row >= _rows.size()) {
        warn("2DA: row index out of range: " + std::to_string(row));
        return std::move(defValue);
    }

    int columnIdx = getColumnIndex(column);
    if (columnIdx == -1) {
        warn("2DA: column not found: " + column);
        return std::move(defValue);
    }

    const std::string &value = _rows[row].values[columnIdx];

    if (value == kCellValueDeleted) {
        warn(boost::format("2DA: cell value was deleted: %d %s") % row % column);
        return std::move(defValue);
    }

    return value;
}

int TwoDa::getInt(int row, const std::string &column, int defValue) const {
    const std::string &value = getString(row, column);
    if (value.empty())
        return defValue;

    return stoi(value);
}

uint32_t TwoDa::getUint(int row, const std::string &column, uint32_t defValue) const {
    const std::string &value = getString(row, column);
    if (value.empty())
        return defValue;

    return stoi(value, nullptr, 16);
}

float TwoDa::getFloat(int row, const std::string &column, float defValue) const {
    const std::string &value = getString(row, column);
    if (value.empty())
        return defValue;

    return stof(value);
}

bool TwoDa::getBool(int row, const std::string &column, bool defValue) const {
    const std::string &value = getString(row, column);
    if (value.empty())
        return defValue;

    return stoi(value) != 0;
}

} // namespace resource

} // namespace reone
