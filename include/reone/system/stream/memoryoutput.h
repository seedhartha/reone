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

#pragma once

#include "output.h"

namespace reone {

class MemoryOutputStream : public IOutputStream {
public:
    MemoryOutputStream(ByteArray &bytes) :
        _buffer(bytes) {
    }

    void writeByte(uint8_t val) override {
        _buffer.push_back(*reinterpret_cast<char *>(&val));
    }

    void write(const char *buf, int len) override {
        size_t pos = _buffer.size();
        _buffer.resize(pos + static_cast<size_t>(len));
        std::memcpy(&_buffer[pos], buf, len);
    }

    size_t position() override {
        return _buffer.size();
    }

private:
    ByteArray &_buffer;
};

} // namespace reone
