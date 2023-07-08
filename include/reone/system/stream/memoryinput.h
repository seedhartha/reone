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
        _data(&str[0]),
        _size(str.size()) {
    }

    MemoryInputStream(ByteArray &bytes) :
        _data(!bytes.empty() ? &bytes[0] : nullptr),
        _size(bytes.size()) {
    }

    void seek(int64_t offset, SeekOrigin origin) override;

    int readByte() override;
    int read(char *outData, int length) override;
    void readLine(char *outData, int maxLen) override;

    size_t position() override {
        return _position;
    }

    bool eof() override {
        return _position >= _size;
    }

private:
    char *_data;
    size_t _size;

    size_t _position {0};
};

} // namespace reone
