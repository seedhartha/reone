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

#pragma once

#include "input.h"

namespace reone {

class ByteArrayInputStream : public IInputStream {
public:
    ByteArrayInputStream(ByteArray &bytes) :
        _bytes(bytes) {
    }

    size_t position() override {
        return _position;
    }

    int readByte() override {
        if (_position >= _bytes.size()) {
            return -1;
        }
        return _bytes[_position++];
    }

    int read(int length, ByteArray &outBytes) override {
        size_t available = _bytes.size() - _position;
        size_t toRead = std::min(available, static_cast<size_t>(length));
        outBytes.resize(std::max(toRead, outBytes.size()));
        std::copy(_bytes.begin() + _position, _bytes.begin() + _position + toRead, outBytes.begin());
        return toRead;
    }

private:
    ByteArray &_bytes;
    size_t _position {0};
};

} // namespace reone
