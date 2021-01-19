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

#include "binfile.h"

#include <unordered_map>

namespace reone {

namespace resource {

class TwoDaRow {
public:
    void add(const std::string &column, const std::string &value);

    const std::string &getString(const std::string &column) const;
    int getInt(const std::string &column) const;
    float getFloat(const std::string &column) const;

    const std::unordered_map<std::string, std::string> &values() const;

private:
    std::unordered_map<std::string, std::string> _values;
};

class TwoDaTable {
public:
    TwoDaTable() = default;

    const TwoDaRow *findRow(const std::function<bool(const TwoDaRow &)> &pred) const;
    const TwoDaRow *findRowByColumnValue(const std::string &columnName, const std::string &columnValue) const;
    const std::string &getString(int row, const std::string &column) const;
    int getInt(int row, const std::string &column, int defValue = 0) const;
    uint32_t getUint(int row, const std::string &column, uint32_t defValue = 0) const;
    float getFloat(int row, const std::string &column, float defValue = 0.0f) const;

    const std::vector<std::string> &headers() const;
    const std::vector<TwoDaRow> &rows() const;

private:
    std::vector<std::string> _headers;
    std::vector<TwoDaRow> _rows;

    TwoDaTable(const TwoDaTable &) = delete;
    TwoDaTable &operator=(const TwoDaTable &) = delete;

    friend class TwoDaFile;
};

class TwoDaFile : public BinaryFile {
public:
    TwoDaFile();

    const std::shared_ptr<TwoDaTable> &table() const;

private:
    int _rowCount { 0 };
    int _dataSize { 0 };
    std::shared_ptr<TwoDaTable> _table;

    void doLoad() override;
    void loadHeaders();
    bool readToken(std::string &token);
    void loadLabels();
    void loadRows();
};

} // namespace resource

} // namespace reone
