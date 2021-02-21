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

#include <stdexcept>

#include <boost/filesystem/fstream.hpp>

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static const char kSignature[] = "2DA V2.b";

TwoDaWriter::TwoDaWriter(const shared_ptr<TwoDA> &twoDa) : _twoDa(twoDa) {
    if (!twoDa) {
        throw invalid_argument("twoDa must not be null");
    }
}

void TwoDaWriter::save(const fs::path &path) {
    auto stream = make_shared<fs::ofstream>(path, ios::binary);

    StreamWriter writer(stream);
    writer.putString(kSignature);
    writer.putChar('\n');

    writeHeaders(writer);

    writer.putUint32(_twoDa->getRowCount());

    writeLabels(writer);
    writeData(writer);
}

void TwoDaWriter::writeHeaders(StreamWriter &writer) {
    for (auto &header : _twoDa->columns()) {
        writer.putString(header);
        writer.putChar('\t');
    }
    writer.putChar('\0');
}

void TwoDaWriter::writeLabels(StreamWriter &writer) {
    for (int i = 0; i < _twoDa->getRowCount(); ++i) {
        writer.putString(to_string(i));
        writer.putChar('\t');
    }
}

void TwoDaWriter::writeData(StreamWriter &writer) {
    vector<pair<string, int>> data;
    int dataSize = 0;

    size_t columnCount = _twoDa->columns().size();
    size_t cellCount = columnCount * _twoDa->getRowCount();

    for (int i = 0; i < _twoDa->getRowCount(); ++i) {
        for (size_t j = 0; j < columnCount; ++j) {
            const string &value = _twoDa->rows()[i].values[j];
            auto maybeData = find_if(data.begin(), data.end(), [&](auto &pair) { return pair.first == value; });
            if (maybeData != data.end()) {
                writer.putUint16(maybeData->second);
            } else {
                data.push_back(make_pair(value, dataSize));
                writer.putUint16(dataSize);
                int len = strnlen(&value[0], value.length());
                dataSize += len + 1ll;
            }
        }
    }

    writer.putUint16(dataSize);

    for (auto &pair : data) {
        writer.putCString(pair.first);
    }
}

} // namespace resource

} // namespace reone
