/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "command.h"

#include <cstring>
#include <stdexcept>
#include <string>

using namespace std;

namespace reone {

namespace mp {

static void putString(const string &s, ByteArray &arr) {
    int off = static_cast<int>(arr.size());
    int len = static_cast<int>(s.length());
    arr.resize(off + len + 1);
    arr[off++] = len;
    if (len > 0) {
        memcpy(&arr[off], &s[0], len);
    }
}

static void putUint8(uint8_t val, ByteArray &arr) {
    arr.push_back(val);
}

static void putUint16(uint16_t val, ByteArray &arr) {
    arr.push_back(val & 0xff);
    arr.push_back((val >> 8) & 0xff);
}

static void putUint32(uint32_t val, ByteArray &arr) {
    arr.push_back(val & 0xff);
    arr.push_back((val >> 8) & 0xff);
    arr.push_back((val >> 16) & 0xff);
    arr.push_back((val >> 24) & 0xff);
}

static void putFloat(float val, ByteArray &arr) {
    uint32_t intVal = *reinterpret_cast<uint32_t *>(&val);
    arr.push_back(intVal & 0xff);
    arr.push_back((intVal >> 8) & 0xff);
    arr.push_back((intVal >> 16) & 0xff);
    arr.push_back((intVal >> 24) & 0xff);
}

static string getString(const ByteArray &arr, int &offset) {
    int len = arr[offset++];
    string s(&arr[offset], len);
    offset += len;
    return move(s);
}

static uint8_t getUint8(const ByteArray &arr, int &offset) {
    return arr[offset++];
}

static uint16_t getUint16(const ByteArray &arr, int &offset) {
    uint16_t val = *reinterpret_cast<const uint16_t *>(&arr[offset]);
    offset += sizeof(uint16_t);
    return val;
}

static uint32_t getUint32(const ByteArray &arr, int &offset) {
    uint32_t val = *reinterpret_cast<const uint32_t *>(&arr[offset]);
    offset += sizeof(uint32_t);
    return val;
}

static float getFloat(const ByteArray &arr, int &offset) {
    float val = *reinterpret_cast<const float *>(&arr[offset]);
    offset += sizeof(float);
    return val;
}

Command::Command(uint32_t id, CommandType type) : net::Command(id), _type(type) {
}

void Command::load(const ByteArray &data) {
    _type = static_cast<CommandType>(data[0]);

    int offset = 1;

    _id = getUint32(data, offset);

    switch (_type) {
        default:
            throw runtime_error("Command: unsupported type: " + to_string(static_cast<int>(_type)));
    }
}

ByteArray Command::getBytes() const {
    ByteArray data;
    putUint8(static_cast<uint8_t>(_type), data);
    putUint32(_id, data);

    switch (_type) {
        default:
            throw runtime_error("Command: unsupported type: " + to_string(static_cast<int>(_type)));
    }

    return move(data);
}

CommandType Command::type() const {
    return _type;
}

} // namespace mp

} // namespace reone
