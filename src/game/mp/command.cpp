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

Command::Command(uint32_t id, net::CommandType type) : net::Command(id, type) {
}

void Command::load(const ByteArray &data) {
    _type = static_cast<net::CommandType>(data[0]);

    int offset = 1;
    int equipmentCount = 0;

    _id = getUint32(data, offset);

    switch (_type) {
        case net::CommandType::LoadModule:
            _module = getString(data, offset);
            break;

        case net::CommandType::LoadCreature:
            _role = static_cast<CreatureRole>(getUint8(data, offset));
            _objectId = getUint16(data, offset);
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

        case net::CommandType::SetPlayerRole:
            _role = static_cast<CreatureRole>(getUint8(data, offset));
            break;

        case net::CommandType::SetObjectTransform:
            _objectId = getUint16(data, offset);
            _position.x = getFloat(data, offset);
            _position.y = getFloat(data, offset);
            _position.z = getFloat(data, offset);
            _heading = getFloat(data, offset);
            break;

        case net::CommandType::SetObjectAnimation:
            _objectId = getUint16(data, offset);
            _animationFlags = getUint8(data, offset);
            _animation = getString(data, offset);
            break;

        case net::CommandType::SetCreatureMovementType:
            _objectId = getUint16(data, offset);
            _movementType = static_cast<MovementType>(getUint8(data, offset));
            break;

        case net::CommandType::SetCreatureTalking:
            _objectId = getUint16(data, offset);
            _talking = getUint8(data, offset);
            break;

        case net::CommandType::SetDoorOpen:
            _open = getUint8(data, offset);
            _objectId = getUint16(data, offset);
            _triggerrer = getUint16(data, offset);
            break;

        case net::CommandType::StartDialog:
            _resRef = getString(data, offset);
            break;

        case net::CommandType::PickDialogReply:
            _replyIndex = getUint8(data, offset);
            break;

        case net::CommandType::FinishDialog:
            break;

        default:
            throw runtime_error("Command: unsupported type: " + to_string(static_cast<int>(_type)));
    }
}

ByteArray Command::bytes() const {
    ByteArray data;
    putUint8(static_cast<uint8_t>(_type), data);
    putUint32(_id, data);

    switch (_type) {
        case net::CommandType::LoadModule:
            putString(_module, data);
            break;

        case net::CommandType::LoadCreature:
            putUint8(static_cast<uint8_t>(_role), data);
            putUint16(_objectId, data);
            putString(_tag, data);
            putUint16(_appearance, data);
            putFloat(_position.x, data);
            putFloat(_position.y, data);
            putFloat(_position.z, data);
            putFloat(_heading, data);
            putUint8(static_cast<uint8_t>(_equipment.size()), data);
            for (auto &item : _equipment) {
                putString(item, data);
            }
            break;

        case net::CommandType::SetPlayerRole:
            putUint8(static_cast<uint8_t>(_role), data);
            break;

        case net::CommandType::SetObjectTransform:
            putUint16(_objectId, data);
            putFloat(_position.x, data);
            putFloat(_position.y, data);
            putFloat(_position.z, data);
            putFloat(_heading, data);
            break;

        case net::CommandType::SetObjectAnimation:
            putUint16(_objectId, data);
            putUint8(_animationFlags, data);
            putString(_animation, data);
            break;

        case net::CommandType::SetCreatureMovementType:
            putUint16(_objectId, data);
            putUint8(static_cast<uint8_t>(_movementType), data);
            break;

        case net::CommandType::SetCreatureTalking:
            putUint16(_objectId, data);
            putUint8(_talking, data);
            break;

        case net::CommandType::SetDoorOpen:
            putUint8(_open, data);
            putUint16(_objectId, data);
            putUint16(_triggerrer, data);
            break;

        case net::CommandType::StartDialog:
            putString(_resRef, data);
            break;

        case net::CommandType::PickDialogReply:
            putUint8(_replyIndex, data);
            break;

        case net::CommandType::FinishDialog:
            break;

        default:
            throw runtime_error("Command: unsupported type: " + to_string(static_cast<int>(_type)));
    }

    return move(data);
}

const string &Command::module() const {
    return _module;
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

bool Command::talking() const {
    return _talking;
}

bool Command::open() const {
    return _open;
}

uint32_t Command::triggerrer() const {
    return _triggerrer;
}

const string &Command::resRef() const {
    return _resRef;
}

uint32_t Command::replyIndex() const {
    return _replyIndex;
}

} // namespace game

} // namespace reone
