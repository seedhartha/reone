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

#include "reone/system/binaryreader.h"
#include "reone/system/stream/input.h"

#include "../2da.h"

namespace reone {

namespace resource {

class TwoDAReader : boost::noncopyable {
public:
    TwoDAReader(IInputStream &stream) :
        _reader(BinaryReader(stream)) {
    }

    void load();

    const std::shared_ptr<TwoDA> &twoDa() const { return _twoDa; }

private:
    BinaryReader _reader;

    int _rowCount {0};
    int _dataSize {0};

    std::vector<std::string> _columns;
    std::vector<TwoDA::Row> _rows;

    std::shared_ptr<TwoDA> _twoDa;

    void loadRows();
    void loadTable();

    std::vector<std::string> readTokens(int maxCount = std::numeric_limits<int>::max());
};

} // namespace resource

} // namespace reone
