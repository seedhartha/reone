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

#pragma once

namespace reone {

namespace resource {

class TwoDaReader;

/**
 * Two-dimensional array, similar to a database table.
 */
class TwoDa : boost::noncopyable {
public:
    struct Row {
        std::vector<std::string> values;
    };

    class Builder : boost::noncopyable {
    public:
        Builder &column(std::string column) {
            _columns.push_back(std::move(column));
            return *this;
        }

        Builder &row(TwoDa::Row row) {
            _rows.push_back(std::move(row));
            return *this;
        }

        Builder &row(std::vector<std::string> values) {
            _rows.push_back(Row {std::move(values)});
            return *this;
        }

        std::unique_ptr<TwoDa> build() {
            return std::make_unique<TwoDa>(_columns, _rows);
        }

    private:
        std::vector<std::string> _columns;
        std::vector<Row> _rows;
    };

    TwoDa(std::vector<std::string> columns, std::vector<Row> rows) :
        _columns(std::move(columns)),
        _rows(std::move(rows)) {
    }

    /**
     * @return index of the first 2DA row, whose cell value equals the specified value, -1 otherwise
     */
    int indexByCellValue(const std::string &column, const std::string &value) const;

    /**
     * @return index of the first 2DA row, whose cell values equal the specified values, -1 otherwise
     */
    int indexByCellValues(const std::vector<std::pair<std::string, std::string>> &values) const;

    int getColumnCount() const { return static_cast<int>(_columns.size()); }
    int getRowCount() const { return static_cast<int>(_rows.size()); }

    std::string getString(int row, const std::string &column, std::string defValue = "") const;
    int getInt(int row, const std::string &column, int defValue = 0) const;
    uint32_t getUint(int row, const std::string &column, uint32_t defValue = 0) const;
    float getFloat(int row, const std::string &column, float defValue = 0.0f) const;
    bool getBool(int row, const std::string &column, bool defValue = false) const;

    const std::vector<std::string> &columns() const { return _columns; }
    const std::vector<Row> &rows() const { return _rows; }

    static Row newRow(std::vector<std::string> values) {
        auto row = Row();
        row.values = std::move(values);
        return std::move(row);
    }

private:
    std::vector<std::string> _columns;
    std::vector<Row> _rows;

    int getColumnIndex(const std::string &column) const;
    std::vector<int> getColumnIndices(const std::vector<std::string> &columns) const;
};

} // namespace resource

} // namespace reone
