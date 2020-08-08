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

#include <stdexcept>

using namespace std;

namespace reone {

namespace game {

static void putString(const string &s, ByteArray &arr) {
    int off = arr.size();
    int len = s.length();
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

static float getFloat(const ByteArray &arr, int &offset) {
    float val = *reinterpret_cast<const float *>(&arr[offset]);
    offset += sizeof(float);
    return val;
}

Command::Command(CommandType type) : _type(type) {
}

void Command::load(const ByteArray &data) {
    _type = static_cast<CommandType>(data[0]);

    int offset = 1;
    int equipmentCount = 0;

    switch (_type) {
        case CommandType::LoadModule:
            _module = getString(data, offset);
            break;

        case CommandType::LoadCreature:
            _role = static_cast<CreatureRole>(getUint8(data, offset));
            _tag = getString(data, offset);
            _appearance = getUint16(data, offset);
            _position.x = getFloat(data, offset);
            _position.y = getFloat(data, offset);
            _position.z = getFloat(data, offset);
            _heading = getFloat(data, offset);
            equipmentCount = getUint8(data, offset);
            for (int i = 0; i < equipmentCount; ++i) {
                _equipment.push_back(getString(data, offset));
            }
            break;

        case CommandType::SetPlayerRole:
            _role = static_cast<CreatureRole>(getUint8(data, offset));
            break;

        case CommandType::SetObjectTransform:
            _tag = getString(data, offset);
            _position.x = getFloat(data, offset);
            _position.y = getFloat(data, offset);
            _position.z = getFloat(data, offset);
            _heading = getFloat(data, offset);
            break;

        case CommandType::SetObjectAnimation:
            _tag = getString(data, offset);
            _animationFlags = getUint8(data, offset);
            _animation = getString(data, offset);
            break;

        case CommandType::SetCreatureMovementType:
            _tag = getString(data, offset);
            _movementType = static_cast<MovementType>(getUint8(data, offset));
            break;

        case CommandType::SetDoorOpen:
            _open = getUint8(data, offset);
            _objectId = getUint16(data, offset);
            _trigerrer = getString(data, offset);
            break;

        default:
            throw runtime_error("Unsupported command type: " + to_string(static_cast<int>(_type)));
    }
}

ByteArray Command::bytes() const {
    ByteArray data;
    putUint8(static_cast<uint8_t>(_type), data);

    switch (_type) {
        case CommandType::LoadModule:
            putString(_module, data);
            break;

        case CommandType::LoadCreature:
            putUint8(static_cast<uint8_t>(_role), data);
            putString(_tag, data);
            putUint16(_appearance, data);
            putFloat(_position.x, data);
            putFloat(_position.y, data);
            putFloat(_position.z, data);
            putFloat(_heading, data);
            putUint8(_equipment.size(), data);
            for (auto &item : _equipment) {
                putString(item, data);
            }
            break;

        case CommandType::SetPlayerRole:
            putUint8(static_cast<uint8_t>(_role), data);
            break;

        case CommandType::SetObjectTransform:
            putString(_tag, data);
            putFloat(_position.x, data);
            putFloat(_position.y, data);
            putFloat(_position.z, data);
            putFloat(_heading, data);
            break;

        case CommandType::SetObjectAnimation:
            putString(_tag, data);
            putUint8(_animationFlags, data);
            putString(_animation, data);
            break;

        case CommandType::SetCreatureMovementType:
            putString(_tag, data);
            putUint8(static_cast<uint8_t>(_movementType), data);
            break;

        case CommandType::SetDoorOpen:
            putUint8(_open, data);
            putUint16(_objectId, data);
            putString(_trigerrer, data);
            break;

        default:
            throw runtime_error("Unsupported command type: " + to_string(static_cast<int>(_type)));
    }

    return move(data);
}

CommandType Command::type() const {
    return _type;
}

const string &Command::module() const {
    return _module;
}

uint32_t Command::objectId() const {
    return _objectId;
}

const string &Command::tag() const {
    return _tag;
}

CreatureRole Command::role() const {
    return _role;
}

int Command::appearance() const {
    return _appearance;
}

const vector<string> &Command::equipment() const {
    return _equipment;
}

const glm::vec3 &Command::position() const {
    return _position;
}

float Command::heading() const {
    return _heading;
}

const string &Command::animation() const {
    return _animation;
}

int Command::animationFlags() const {
    return _animationFlags;
}

MovementType Command::movementType() const {
    return _movementType;
}

bool Command::open() const {
    return _open;
}

const string &Command::trigerrer() const {
    return _trigerrer;
}

} // namespace game

} // namespace reone
