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

#include "reone/system/exception/validation.h"

namespace reone {

namespace resource {

class TwoDAReader;

class TwoDA : boost::noncopyable {
public:
    struct Row {
        std::vector<std::string> values;
    };

    class Builder {
    public:
        Builder &columns(std::vector<std::string> columns) {
            _columns = std::move(columns);
            return *this;
        }

        Builder &row(std::vector<std::string> values) {
            _rows.push_back({std::move(values)});
            return *this;
        }

        std::unique_ptr<TwoDA> build() {
            return std::make_unique<TwoDA>(_columns, _rows);
        }

    private:
        std::vector<std::string> _columns;
        std::vector<Row> _rows;
    };

    TwoDA(std::vector<std::string> columns, std::vector<Row> rows) :
        _columns(std::move(columns)),
        _rows(std::move(rows)) {
        size_t numColumns = _columns.size();
        for (size_t row = 0; row < _rows.size(); ++row) {
            size_t numValues = _rows[row].values.size();
            if (numValues != numColumns) {
                throw ValidationException(str(boost::format("Expected %d columns in 2DA row %d, was %d") % numColumns % row % numValues));
            }
        }
    }

    /**
     * @return row index or -1 when not found
     */
    int indexByCellValue(const std::string &column, const std::string &value) const;

    /**
     * @return row index or -1 when not found
     */
    int indexByCellValues(const std::vector<std::pair<std::string, std::string>> &values) const;

    int getColumnCount() const { return static_cast<int>(_columns.size()); }
    int getRowCount() const { return static_cast<int>(_rows.size()); }

    std::string getString(int row, const std::string &column, std::string defValue = "") const;
    int getInt(int row, const std::string &column, int defValue = 0) const;
    uint32_t getHexUint(int row, const std::string &column, uint32_t defValue = 0) const;
    float getFloat(int row, const std::string &column, float defValue = 0.0f) const;
    bool getBool(int row, const std::string &column, bool defValue = false) const;

    const std::vector<std::string> &columns() const { return _columns; }
    const std::vector<Row> &rows() const { return _rows; }

    static Row newRow(std::vector<std::string> values) {
        auto row = Row();
        row.values = std::move(values);
        return row;
    }

private:
    std::vector<std::string> _columns;
    std::vector<Row> _rows;

    int getColumnIndex(const std::string &column) const;
    std::vector<int> getColumnIndices(const std::vector<std::string> &columns) const;
};

} // namespace resource

} // namespace reone
