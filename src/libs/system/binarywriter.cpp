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

#include "reone/system/binarywriter.h"

using namespace std;

namespace reone {

void BinaryWriter::putByte(uint8_t val) {
    _stream.writeByte(val);
}

void BinaryWriter::putChar(char val) {
    _stream.writeByte(*reinterpret_cast<uint8_t *>(&val));
}

void BinaryWriter::putUint16(uint16_t val) {
    put(val);
}

void BinaryWriter::putUint32(uint32_t val) {
    put(val);
}

void BinaryWriter::putInt16(int16_t val) {
    put(val);
}

void BinaryWriter::putInt32(int32_t val) {
    put(val);
}

void BinaryWriter::putInt64(int64_t val) {
    put(val);
}

void BinaryWriter::putFloat(float val) {
    put(*reinterpret_cast<uint32_t *>(&val));
}

void BinaryWriter::putString(const string &str) {
    _stream.write(&str[0], str.length());
}

void BinaryWriter::putStringExact(const string &str, int len) {
    int strLen = min(len, static_cast<int>(str.length()));
    if (strLen > 0) {
        _stream.write(&str[0], strLen);
    }
    for (int i = 0; i < len - strLen; ++i) {
        _stream.writeByte('\0');
    }
}

void BinaryWriter::putCString(const string &str) {
    int len = static_cast<int>(strnlen(&str[0], str.length()));
    _stream.write(&str[0], len);
    _stream.writeByte('\0');
}

void BinaryWriter::putBytes(const ByteArray &bytes) {
    _stream.write(&bytes[0], bytes.size());
}

void BinaryWriter::putBytes(int count, uint8_t val) {
    ByteArray data(count, val);
    _stream.write(&data[0], count);
}

size_t BinaryWriter::tell() const {
    return _stream.position();
}

} // namespace reone
