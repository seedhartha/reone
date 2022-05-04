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

#include "2dawriter.h"

#include "../../common/stream/fileoutput.h"

#include "../2da.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static const char kSignature[] = "2DA V2.b";

void TwoDaWriter::save(const fs::path &path) {
    auto out = FileOutputStream(path, OpenMode::Binary);
    save(out);
}

void TwoDaWriter::save(IOutputStream &out) {
    _writer = make_unique<BinaryWriter>(out);
    _writer->putString(kSignature);
    _writer->putChar('\n');

    writeHeaders();

    _writer->putUint32(_twoDa.getRowCount());

    writeLabels();
    writeData();
}

void TwoDaWriter::writeHeaders() {
    for (auto &column : _twoDa.columns()) {
        _writer->putString(column);
        _writer->putChar('\t');
    }
    _writer->putChar('\0');
}

void TwoDaWriter::writeLabels() {
    for (int i = 0; i < _twoDa.getRowCount(); ++i) {
        _writer->putString(to_string(i));
        _writer->putChar('\t');
    }
}

void TwoDaWriter::writeData() {
    vector<pair<string, int>> data;
    int dataSize = 0;

    size_t columnCount = _twoDa.columns().size();
    size_t cellCount = columnCount * _twoDa.getRowCount();

    for (int i = 0; i < _twoDa.getRowCount(); ++i) {
        for (size_t j = 0; j < columnCount; ++j) {
            const string &value = _twoDa.rows()[i].values[j];
            auto maybeData = find_if(data.begin(), data.end(), [&](auto &pair) { return pair.first == value; });
            if (maybeData != data.end()) {
                _writer->putUint16(maybeData->second);
            } else {
                data.push_back(make_pair(value, dataSize));
                _writer->putUint16(dataSize);
                int len = static_cast<int>(strnlen(&value[0], value.length()));
                dataSize += len + 1;
            }
        }
    }

    _writer->putUint16(dataSize);

    for (auto &pair : data) {
        _writer->putCString(pair.first);
    }
}

} // namespace resource

} // namespace reone
