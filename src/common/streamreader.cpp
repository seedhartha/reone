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

#include "streamreader.h"

using namespace std;

namespace endian = boost::endian;

namespace reone {

size_t StreamReader::tell() {
    return _stream.tellg();
}

void StreamReader::seek(size_t pos) {
    _stream.clear();
    _stream.seekg(pos);
}

void StreamReader::ignore(int count) {
    _stream.ignore(count);
}

uint8_t StreamReader::getByte() {
    uint8_t val;
    _stream.read(reinterpret_cast<char *>(&val), 1);
    return val;
}

uint16_t StreamReader::getUint16() {
    uint16_t val;
    _stream.read(reinterpret_cast<char *>(&val), 2);
    endian::conditional_reverse_inplace(val, _endianess, endian::order::native);
    return val;
}

uint32_t StreamReader::getUint32() {
    uint32_t val;
    _stream.read(reinterpret_cast<char *>(&val), 4);
    endian::conditional_reverse_inplace(val, _endianess, endian::order::native);
    return val;
}

uint64_t StreamReader::getUint64() {
    uint64_t val;
    _stream.read(reinterpret_cast<char *>(&val), 8);
    endian::conditional_reverse_inplace(val, _endianess, endian::order::native);
    return val;
}

int16_t StreamReader::getInt16() {
    int16_t val;
    _stream.read(reinterpret_cast<char *>(&val), 2);
    endian::conditional_reverse_inplace(val, _endianess, endian::order::native);
    return val;
}

int32_t StreamReader::getInt32() {
    int32_t val;
    _stream.read(reinterpret_cast<char *>(&val), 4);
    endian::conditional_reverse_inplace(val, _endianess, endian::order::native);
    return val;
}

int64_t StreamReader::getInt64() {
    int64_t val;
    _stream.read(reinterpret_cast<char *>(&val), 8);
    endian::conditional_reverse_inplace(val, _endianess, endian::order::native);
    return val;
}

float StreamReader::getFloat() {
    uint32_t val;
    _stream.read(reinterpret_cast<char *>(&val), 4);
    endian::conditional_reverse_inplace(val, _endianess, endian::order::native);
    return *reinterpret_cast<float *>(&val);
}

double StreamReader::getDouble() {
    uint64_t val;
    _stream.read(reinterpret_cast<char *>(&val), 8);
    endian::conditional_reverse_inplace(val, _endianess, endian::order::native);
    return *reinterpret_cast<double *>(&val);
}

string StreamReader::getString(int len) {
    string val;
    val.resize(len);
    _stream.read(&val[0], len);
    return move(val);
}

string StreamReader::getNullTerminatedString() {
    stringbuf ss;
    _stream.get(ss, '\0');
    _stream.seekg(1, ios::cur);
    return ss.str();
}

u16string StreamReader::getNullTerminatedStringUTF16() {
    basic_stringstream<char16_t> ss;

    char16_t ch;
    do {
        ch = getUint16();
        if (ch) {
            ss.put(ch);
        }
    } while (ch);

    return ss.str();
}

ByteArray StreamReader::getBytes(int count) {
    ByteArray result;
    result.resize(count);
    _stream.read(reinterpret_cast<char *>(&result[0]), count);
    return move(result);
}

bool StreamReader::eof() const {
    return _stream.eof();
}

} // namespace reone
