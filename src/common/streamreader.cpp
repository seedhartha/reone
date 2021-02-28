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

#include <sstream>
#include <stdexcept>

#include "endianutil.h"

using namespace std;

namespace reone {

StreamReader::StreamReader(const shared_ptr<istream> &stream, Endianess endianess) :
    _stream(stream),
    _endianess(endianess) {

    if (!stream) {
        throw invalid_argument("stream must not be null");
    }
}

size_t StreamReader::tell() {
    return _stream->tellg();
}

void StreamReader::seek(size_t pos) {
    _stream->clear();
    _stream->seekg(pos);
}

void StreamReader::ignore(int count) {
    _stream->ignore(count);
}

uint8_t StreamReader::getByte() {
    uint8_t val;
    _stream->read(reinterpret_cast<char *>(&val), 1);
    return val;
}

uint16_t StreamReader::getUint16() {
    uint16_t val;
    _stream->read(reinterpret_cast<char *>(&val), 2);
    swapBytesIfNotSystemEndianess(val, _endianess);
    return val;
}

uint32_t StreamReader::getUint32() {
    uint32_t val;
    _stream->read(reinterpret_cast<char *>(&val), 4);
    swapBytesIfNotSystemEndianess(val, _endianess);
    return val;
}

uint64_t StreamReader::getUint64() {
    uint64_t val;
    _stream->read(reinterpret_cast<char *>(&val), 8);
    swapBytesIfNotSystemEndianess(val, _endianess);
    return val;
}

int16_t StreamReader::getInt16() {
    int16_t val;
    _stream->read(reinterpret_cast<char *>(&val), 2);
    swapBytesIfNotSystemEndianess(val, _endianess);
    return val;
}

int32_t StreamReader::getInt32() {
    int32_t val;
    _stream->read(reinterpret_cast<char *>(&val), 4);
    swapBytesIfNotSystemEndianess(val, _endianess);
    return val;
}

int64_t StreamReader::getInt64() {
    int64_t val;
    _stream->read(reinterpret_cast<char *>(&val), 8);
    swapBytesIfNotSystemEndianess(val, _endianess);
    return val;
}

float StreamReader::getFloat() {
    float val;
    _stream->read(reinterpret_cast<char *>(&val), 4);
    swapBytesIfNotSystemEndianess(val, _endianess);
    return val;
}

double StreamReader::getDouble() {
    double val;
    _stream->read(reinterpret_cast<char *>(&val), 8);
    swapBytesIfNotSystemEndianess(val, _endianess);
    return val;
}

string StreamReader::getCString() {
    stringbuf ss;
    _stream->get(ss, '\0');
    _stream->seekg(1, ios::cur);
    return ss.str();
}

string StreamReader::getString(int len) {
    string val;
    val.resize(len);
    _stream->read(&val[0], len);
    return move(val);
}

void StreamReader::read(char &obj) {
    _stream->get(obj);
}

void StreamReader::read(string &obj) {
    obj = getCString();
}

bool StreamReader::eof() const {
    return _stream->eof();
}

template <>
vector<char> StreamReader::getArray(int count) {
    vector<char> result(count);
    if (count > 0) {
        _stream->read(&result[0], count);
    }
    return move(result);
}

template <>
vector<uint8_t> StreamReader::getArray(int count) {
    vector<uint8_t> result(count);
    if (count > 0) {
        _stream->read(reinterpret_cast<char *>(&result[0]), count);
    }
    return move(result);
}

template <>
vector<uint16_t> StreamReader::getArray(int count) {
    vector<uint16_t> result(count);
    for (int i = 0; i < count; ++i) {
        result[i] = getUint16();
    }
    return move(result);
}

template <>
vector<uint32_t> StreamReader::getArray(int count) {
    vector<uint32_t> result(count);
    for (int i = 0; i < count; ++i) {
        result[i] = getUint32();
    }
    return move(result);
}

template <>
vector<float> StreamReader::getArray(int count) {
    vector<float> result(count);
    for (int i = 0; i < count; ++i) {
        result[i] = getFloat();
    }
    return move(result);
}

} // namespace reone
