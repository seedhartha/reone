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

#include "streamwriter.h"

#include <stdexcept>

#include "endianutil.h"

using namespace std;

namespace reone {

StreamWriter::StreamWriter(const shared_ptr<ostream> &stream, Endianess endianess) :
    _stream(stream),
    _endianess(endianess) {

    if (!stream) {
        throw invalid_argument("stream must not be null");
    }
}

void StreamWriter::putByte(uint8_t val) {
    _stream->put(val);
}

void StreamWriter::putChar(char val) {
    _stream->put(val);
}

void StreamWriter::putUint16(uint16_t val) {
    put(val);
}

void StreamWriter::putUint32(uint32_t val) {
    put(val);
}

void StreamWriter::putInt64(int64_t val) {
    put(val);
}

void StreamWriter::putString(const string &str) {
    int len = strnlen(&str[0], str.length());
    _stream->write(&str[0], len);
}

void StreamWriter::putCString(const string &str) {
    int len = strnlen(&str[0], str.length());
    _stream->write(&str[0], len);
    _stream->put('\0');
}

size_t StreamWriter::tell() const {
    return _stream->tellp();
}

} // namespace reone
