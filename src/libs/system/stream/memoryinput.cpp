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

#include "reone/system/stream/memoryinput.h"

namespace reone {

void MemoryInputStream::seek(int64_t offset, SeekOrigin origin) {
    if (origin == SeekOrigin::Begin) {
        _position = offset;
    } else if (origin == SeekOrigin::Current) {
        _position += offset;
    } else if (origin == SeekOrigin::End) {
        _position = _size - offset;
    } else {
        throw std::invalid_argument("Unsupported origin: " + std::to_string(static_cast<int>(origin)));
    }
}

int MemoryInputStream::readByte() {
    if (_position >= _size) {
        return -1;
    }
    return _data[_position++];
}

int MemoryInputStream::read(char *outData, int length) {
    size_t available = _size - _position;
    size_t toRead = std::min(available, static_cast<size_t>(length));
    std::memcpy(outData, &_data[_position], toRead);
    if (toRead < length) {
        _position = _size;
    } else {
        _position += length;
    }
    return toRead;
}

void MemoryInputStream::readLine(char *outData, int maxLen) {
    size_t endPos;
    for (endPos = _position; endPos < _size; ++endPos) {
        if (_data[endPos] == '\r' || _data[endPos] == '\n') {
            break;
        }
    }
    size_t toRead = std::min(endPos - _position, static_cast<size_t>(maxLen));
    std::memcpy(outData, &_data[_position], toRead);
    if (toRead < maxLen) {
        outData[toRead] = '\0';
    }
    _position += toRead;
    if (endPos >= _size) {
        // do nothing
    } else if (_data[endPos] == '\r') {
        _position += 2;
    } else if (_data[endPos] == '\n') {
        _position += 1;
    }
}

} // namespace reone
