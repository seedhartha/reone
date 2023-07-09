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

#include "reone/resource/format/2dawriter.h"

#include "reone/system/stream/fileoutput.h"

#include "reone/resource/2da.h"

namespace reone {

namespace resource {

static const char kSignature[] = "2DA V2.b";

void TwoDaWriter::save(const boost::filesystem::path &path) {
    auto out = FileOutputStream(path);
    save(out);
}

void TwoDaWriter::save(IOutputStream &out) {
    _writer = std::make_unique<BinaryWriter>(out);
    _writer->writeString(kSignature);
    _writer->writeChar('\n');

    writeHeaders();

    _writer->writeUint32(_twoDa.getRowCount());

    writeLabels();
    writeData();
}

void TwoDaWriter::writeHeaders() {
    for (auto &column : _twoDa.columns()) {
        _writer->writeString(column);
        _writer->writeChar('\t');
    }
    _writer->writeChar('\0');
}

void TwoDaWriter::writeLabels() {
    for (int i = 0; i < _twoDa.getRowCount(); ++i) {
        _writer->writeString(std::to_string(i));
        _writer->writeChar('\t');
    }
}

void TwoDaWriter::writeData() {
    std::vector<std::pair<std::string, int>> data;
    int dataSize = 0;

    size_t columnCount = _twoDa.columns().size();
    size_t cellCount = columnCount * _twoDa.getRowCount();

    for (int i = 0; i < _twoDa.getRowCount(); ++i) {
        for (size_t j = 0; j < columnCount; ++j) {
            const std::string &value = _twoDa.rows()[i].values[j];
            auto maybeData = std::find_if(data.begin(), data.end(), [&](auto &pair) { return pair.first == value; });
            if (maybeData != data.end()) {
                _writer->writeUint16(maybeData->second);
            } else {
                data.push_back(std::make_pair(value, dataSize));
                _writer->writeUint16(dataSize);
                int len = static_cast<int>(strnlen(&value[0], value.length()));
                dataSize += len + 1;
            }
        }
    }

    _writer->writeUint16(dataSize);

    for (auto &pair : data) {
        _writer->writeCString(pair.first);
    }
}

} // namespace resource

} // namespace reone
