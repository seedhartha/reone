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

int MemoryInputStream::read(char *outData, int length) {
    size_t available = _length - _position;
    size_t numRead = std::min(available, static_cast<size_t>(length));
    std::memcpy(outData, &_data[_position], numRead);
    if (numRead < length) {
        _position = _length;
    } else {
        _position += length;
    }
    return numRead;
}

} // namespace reone
