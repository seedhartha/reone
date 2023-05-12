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

class ByteArrayInputStream : public IInputStream {
public:
    ByteArrayInputStream(ByteArray &bytes) :
        _bytes(bytes) {
    }

    void seek(int64_t offset, SeekOrigin origin) override {
        if (origin == SeekOrigin::Begin) {
            _position = offset;
        } else if (origin == SeekOrigin::Current) {
            _position += offset;
        } else if (origin == SeekOrigin::End) {
            _position = _bytes.size() - offset;
        } else {
            throw std::invalid_argument("Unsupported origin: " + std::to_string(static_cast<int>(origin)));
        }
    }

    int readByte() override {
        if (_position >= _bytes.size()) {
            return -1;
        }
        return _bytes[_position++];
    }

    int read(char *outData, int length) override {
        size_t available = _bytes.size() - _position;
        size_t toRead = std::min(available, static_cast<size_t>(length));
        std::memcpy(outData, &_bytes[_position], toRead);
        if (toRead < length) {
            _position = _bytes.size();
        } else {
            _position += length;
        }
        return toRead;
    }

    void readLine(char *outData, int maxLen) override {
        auto available = _bytes.size() - _position;
        auto toRead = std::min(available, static_cast<size_t>(maxLen));
        std::memcpy(outData, &_bytes[_position], toRead);
        if (toRead < maxLen - 1) {
            outData[toRead + 1] = '\0';
        } else {
            outData[maxLen - 1] = '\0';
        }
        char *pch;
        for (pch = outData; *pch && *pch != '\n'; ++pch)
            ;
        auto lineLen = pch - outData;
        outData[lineLen] = '\0';
        _position = std::min(_bytes.size(), _position + lineLen + 1);
    }

    size_t position() override {
        return _position;
    }

    bool eof() override {
        return _position >= _bytes.size();
    }

private:
    ByteArray &_bytes;
    size_t _position {0};
};

} // namespace reone
