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

#include "input.h"

namespace reone {

class MemoryInputStream : public IInputStream {
public:
    MemoryInputStream(std::string &str) :
        _data(!str.empty() ? &str[0] : nullptr),
        _length(str.length()) {
    }

    MemoryInputStream(ByteArray &bytes) :
        _data(!bytes.empty() ? &bytes[0] : nullptr),
        _length(bytes.size()) {
    }

    void seek(int64_t off, SeekOrigin origin) override {
        if (origin == SeekOrigin::Begin) {
            _position = off;
        } else if (origin == SeekOrigin::Current) {
            _position += off;
        } else if (origin == SeekOrigin::End) {
            _position = _length - off;
        } else {
            throw std::invalid_argument("Invalid origin: " + std::to_string(static_cast<int>(origin)));
        }
    }

    int readByte() override {
        if (_position >= _length) {
            return -1;
        }
        return _data[_position++];
    }

    int read(char *buf, int length) override;

    size_t position() override { return _position; }
    size_t length() override { return _length; }

private:
    char *_data;
    size_t _length;

    size_t _position {0};
};

} // namespace reone
