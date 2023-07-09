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

namespace reone {

void BinaryWriter::writeByte(uint8_t val) {
    _stream.writeByte(val);
}

void BinaryWriter::writeChar(char val) {
    _stream.writeByte(*reinterpret_cast<uint8_t *>(&val));
}

void BinaryWriter::writeUint16(uint16_t val) {
    write(val);
}

void BinaryWriter::writeUint32(uint32_t val) {
    write(val);
}

void BinaryWriter::writeInt16(int16_t val) {
    write(val);
}

void BinaryWriter::writeInt32(int32_t val) {
    write(val);
}

void BinaryWriter::writeInt64(int64_t val) {
    write(val);
}

void BinaryWriter::writeFloat(float val) {
    write(*reinterpret_cast<uint32_t *>(&val));
}

void BinaryWriter::writeString(const std::string &str) {
    _stream.write(&str[0], str.length());
}

void BinaryWriter::writeCString(const std::string &str) {
    int len = static_cast<int>(strnlen(&str[0], str.length()));
    _stream.write(&str[0], len);
    _stream.writeByte('\0');
}

void BinaryWriter::write(const ByteBuffer &bytes) {
    _stream.write(&bytes[0], bytes.size());
}

void BinaryWriter::write(int count, uint8_t val) {
    ByteBuffer data(count, val);
    _stream.write(&data[0], count);
}

size_t BinaryWriter::position() const {
    return _stream.position();
}

} // namespace reone
