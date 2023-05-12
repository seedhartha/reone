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

using namespace std;

namespace reone {

size_t BinaryReader::tell() {
    return _stream.position();
}

void BinaryReader::seek(size_t pos, SeekOrigin origin) {
    _stream.seek(pos, origin);
}

void BinaryReader::ignore(int count) {
    _stream.seek(count, SeekOrigin::Current);
}

uint8_t BinaryReader::getByte() {
    uint8_t val;
    _stream.read(reinterpret_cast<char *>(&val), 1);
    return val;
}

uint16_t BinaryReader::getUint16() {
    uint16_t val;
    _stream.read(reinterpret_cast<char *>(&val), 2);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

uint32_t BinaryReader::getUint32() {
    uint32_t val;
    _stream.read(reinterpret_cast<char *>(&val), 4);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

uint64_t BinaryReader::getUint64() {
    uint64_t val;
    _stream.read(reinterpret_cast<char *>(&val), 8);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

int16_t BinaryReader::getInt16() {
    int16_t val;
    _stream.read(reinterpret_cast<char *>(&val), 2);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

int32_t BinaryReader::getInt32() {
    int32_t val;
    _stream.read(reinterpret_cast<char *>(&val), 4);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

int64_t BinaryReader::getInt64() {
    int64_t val;
    _stream.read(reinterpret_cast<char *>(&val), 8);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return val;
}

float BinaryReader::getFloat() {
    uint32_t val;
    _stream.read(reinterpret_cast<char *>(&val), 4);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return *reinterpret_cast<float *>(&val);
}

double BinaryReader::getDouble() {
    uint64_t val;
    _stream.read(reinterpret_cast<char *>(&val), 8);
    boost::endian::conditional_reverse_inplace(val, _endianess, boost::endian::order::native);
    return *reinterpret_cast<double *>(&val);
}

string BinaryReader::getString(int len) {
    string val;
    val.resize(len);
    _stream.read(&val[0], len);
    return move(val);
}

string BinaryReader::getNullTerminatedString() {
    ostringstream ss;

    char ch;
    do {
        ch = _stream.readByte();
        if (ch && ch != -1) {
            ss << ch;
        }
    } while (ch);

    return ss.str();
}

ByteArray BinaryReader::getBytes(int count) {
    ByteArray buffer;
    buffer.resize(count);
    int numRead = _stream.read(reinterpret_cast<char *>(&buffer[0]), count);
    buffer.resize(numRead);
    return move(buffer);
}

bool BinaryReader::eof() const {
    return _stream.eof();
}

} // namespace reone
