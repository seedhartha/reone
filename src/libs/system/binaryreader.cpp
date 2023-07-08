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

#include "reone/system/binaryreader.h"

namespace reone {

uint8_t BinaryReader::readByte() {
    uint8_t val;
    _stream.read(reinterpret_cast<char *>(&val), 1);
    return val;
}

char BinaryReader::readChar() {
    char val;
    _stream.read(&val, 1);
    return val;
}

uint16_t BinaryReader::readUint16() {
    uint16_t val;
    _stream.read(reinterpret_cast<char *>(&val), 2);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

uint32_t BinaryReader::readUint32() {
    uint32_t val;
    _stream.read(reinterpret_cast<char *>(&val), 4);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

uint64_t BinaryReader::readUint64() {
    uint64_t val;
    _stream.read(reinterpret_cast<char *>(&val), 8);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

int16_t BinaryReader::readInt16() {
    int16_t val;
    _stream.read(reinterpret_cast<char *>(&val), 2);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

int32_t BinaryReader::readInt32() {
    int32_t val;
    _stream.read(reinterpret_cast<char *>(&val), 4);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

int64_t BinaryReader::readInt64() {
    int64_t val;
    _stream.read(reinterpret_cast<char *>(&val), 8);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

float BinaryReader::readFloat() {
    uint32_t val;
    _stream.read(reinterpret_cast<char *>(&val), 4);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return *reinterpret_cast<float *>(&val);
}

double BinaryReader::readDouble() {
    uint64_t val;
    _stream.read(reinterpret_cast<char *>(&val), 8);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return *reinterpret_cast<double *>(&val);
}

std::string BinaryReader::readString(int len) {
    std::string val;
    val.resize(len);
    _stream.read(&val[0], len);
    return std::move(val);
}

std::string BinaryReader::readNullTerminatedString() {
    std::ostringstream ss;

    char ch;
    do {
        ch = _stream.readByte();
        if (ch && ch != -1) {
            ss << ch;
        }
    } while (ch);

    return ss.str();
}

ByteArray BinaryReader::readBytes(int count) {
    ByteArray buffer;
    buffer.resize(count);
    int numRead = _stream.read(reinterpret_cast<char *>(&buffer[0]), count);
    buffer.resize(numRead);
    return buffer;
}

} // namespace reone
