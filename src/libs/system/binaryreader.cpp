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
#include "reone/system/exception/endofstream.h"

namespace reone {

uint8_t BinaryReader::readByte() {
    return static_cast<uint8_t>(_stream.readByte());
}

char BinaryReader::readChar() {
    char val;
    if (_stream.read(&val, 1) != 1) {
        throw EndOfStreamException();
    }
    return val;
}

uint16_t BinaryReader::readUint16() {
    uint16_t val;
    if (_stream.read(reinterpret_cast<char *>(&val), 2) != 2) {
        throw EndOfStreamException();
    }
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

uint32_t BinaryReader::readUint32() {
    uint32_t val;
    if (_stream.read(reinterpret_cast<char *>(&val), 4) != 4) {
        throw EndOfStreamException();
    }
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

uint64_t BinaryReader::readUint64() {
    uint64_t val;
    if (_stream.read(reinterpret_cast<char *>(&val), 8) != 8) {
        throw EndOfStreamException();
    }
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

int16_t BinaryReader::readInt16() {
    int16_t val;
    if (_stream.read(reinterpret_cast<char *>(&val), 2) != 2) {
        throw EndOfStreamException();
    }
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

int32_t BinaryReader::readInt32() {
    int32_t val;
    if (_stream.read(reinterpret_cast<char *>(&val), 4) != 4) {
        throw EndOfStreamException();
    }
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

int64_t BinaryReader::readInt64() {
    int64_t val;
    if (_stream.read(reinterpret_cast<char *>(&val), 8) != 8) {
        throw EndOfStreamException();
    }
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

float BinaryReader::readFloat() {
    uint32_t val;
    if (_stream.read(reinterpret_cast<char *>(&val), 4) != 4) {
        throw EndOfStreamException();
    }
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return *reinterpret_cast<float *>(&val);
}

double BinaryReader::readDouble() {
    uint64_t val;
    if (_stream.read(reinterpret_cast<char *>(&val), 8) != 8) {
        throw EndOfStreamException();
    }
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return *reinterpret_cast<double *>(&val);
}

std::string BinaryReader::readString(int len) {
    std::vector<char> buf;
    buf.resize(len + 1, '\0');
    if (_stream.read(&buf[0], len) != len) {
        throw EndOfStreamException();
    }
    return std::string(&buf[0]);
}

std::string BinaryReader::readCString(int maxlen) {
    auto pos = _stream.position();

    std::vector<char> buf;
    buf.resize(maxlen, '\0');
    _stream.read(&buf[0], maxlen);

    auto termIter = std::find(buf.begin(), buf.end(), '\0');
    if (termIter == buf.end()) {
        throw std::runtime_error("String not null-terminated");
    }
    auto len = std::distance(buf.begin(), termIter);
    _stream.seek(pos + len + 1);

    return std::string(&buf[0], len);
}

ByteArray BinaryReader::readBytes(int count) {
    ByteArray buf;
    buf.resize(count);
    if (_stream.read(reinterpret_cast<char *>(&buf[0]), count) != count) {
        throw EndOfStreamException();
    }
    return buf;
}

} // namespace reone
