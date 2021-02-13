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

#include <unordered_map>

#include "../../common/streamwriter.h"

#include "binfile.h"

namespace reone {

namespace resource {

class TwoDaRow {
public:
    void add(const std::string &column, const std::string &value);

    const std::string &getString(const std::string &column) const;
    int getInt(const std::string &column, int defValue = 0) const;
    float getFloat(const std::string &column, float defValue = 0.0f) const;
    bool getBool(const std::string &column, bool defValue = false) const;

    const std::vector<std::pair<std::string, std::string>> &values() const { return _values; }

private:
    std::vector<std::pair<std::string, std::string>> _values;
};

class TwoDaTable {
public:
    TwoDaTable() = default;

    void add(TwoDaRow row);

    const TwoDaRow *get(const std::function<bool(const TwoDaRow &)> &pred) const;
    const TwoDaRow *getByColumnValue(const std::string &columnName, const std::string &columnValue) const;
    const std::string &getString(int row, const std::string &column) const;
    int getInt(int row, const std::string &column, int defValue = 0) const;
    uint32_t getUint(int row, const std::string &column, uint32_t defValue = 0) const;
    float getFloat(int row, const std::string &column, float defValue = 0.0f) const;
    bool getBool(int row, const std::string &column, bool defValue = false) const;

    const std::vector<std::string> &headers() const { return _headers; }
    const std::vector<TwoDaRow> &rows() const { return _rows; }

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

    const std::shared_ptr<TwoDaTable> &table() const { return _table; }

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

class TwoDaWriter {
public:
    TwoDaWriter(const std::shared_ptr<TwoDaTable> &table);

    void save(const boost::filesystem::path &path);

private:
    std::shared_ptr<TwoDaTable> _table;

    void writeHeaders(StreamWriter &writer);
    void writeLabels(StreamWriter &writer);
    void writeData(StreamWriter &writer);
};

} // namespace resource

} // namespace reone
