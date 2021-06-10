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

#include "guardutil.h"

using namespace std;

namespace reone {

StreamWriter::StreamWriter(const shared_ptr<ostream> &stream, boost::endian::order endianess) :
    _stream(stream),
    _endianess(endianess) {

    ensureNotNull(stream, "stream");
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

void StreamWriter::putInt32(int32_t val) {
    put(val);
}

void StreamWriter::putInt64(int64_t val) {
    put(val);
}

void StreamWriter::putFloat(float val) {
    put(*reinterpret_cast<uint32_t *>(&val));
}

void StreamWriter::putString(const string &str) {
    _stream->write(&str[0], str.length());
}

void StreamWriter::putCString(const string &str) {
    int len = static_cast<int>(strnlen(&str[0], str.length()));
    _stream->write(&str[0], len);
    _stream->put('\0');
}

void StreamWriter::putBytes(const ByteArray &bytes) {
    _stream->write(&bytes[0], bytes.size());
}

void StreamWriter::putBytes(int count, uint8_t val) {
    ByteArray data(count, val);
    _stream->write(&data[0], count);
}

size_t StreamWriter::tell() const {
    return _stream->tellp();
}

} // namespace reone
