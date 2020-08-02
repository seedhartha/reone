#pragma once

#include "binfile.h"

namespace reone {

namespace resources {

struct TwoDaRow {
    std::vector<std::string> values;
};

class TwoDaTable {
public:
    TwoDaTable() = default;

    const std::string &getString(int row, const std::string &column) const;
    int getInt(int row, const std::string &column, int defValue) const;
    uint32_t getUint(int row, const std::string &column, uint32_t defValue) const;
    float getFloat(int row, const std::string &column, float defValue) const;
    const std::string &getStringFromRowByColumnValue(const std::string &column, const std::string &rowByColumn, const std::string &columnValue, const std::string &defValue) const;

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

} // namespace resources

} // namespace reone
