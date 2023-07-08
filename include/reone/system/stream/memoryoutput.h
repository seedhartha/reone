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
        _bytes(bytes) {
    }

    void writeByte(char c) override {
        _bytes.push_back(c);
    }

    void write(const ByteArray &bytes) override {
        _bytes.insert(_bytes.end(), bytes.begin(), bytes.end());
    }

    void write(const std::string &str) override {
        _bytes.insert(_bytes.end(), str.begin(), str.end());
    }

    void write(const char *data, int length) override {
        size_t position = _bytes.size();
        _bytes.resize(position + static_cast<size_t>(length));
        std::memcpy(&_bytes[position], data, length);
    }

    size_t position() override {
        return _bytes.size();
    }

private:
    ByteArray &_bytes;
};

} // namespace reone
