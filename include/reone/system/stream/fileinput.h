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

#include "reone/system/exception/notimplemented.h"

#include "input.h"

namespace reone {

class FileInputStream : public IInputStream {
public:
    FileInputStream(const std::filesystem::path &path) :
        _stream(path, std::ios::binary) {
    }

    void seek(int64_t offset, SeekOrigin origin) override {
        _stream.clear();
        if (origin == SeekOrigin::Begin) {
            _stream.seekg(offset, std::ios::beg);
        } else if (origin == SeekOrigin::Current) {
            _stream.seekg(offset, std::ios::cur);
        } else if (origin == SeekOrigin::End) {
            _stream.seekg(offset, std::ios::end);
        } else {
            throw std::invalid_argument("Invalid origin: " + std::to_string(static_cast<int>(origin)));
        }
    }

    int readByte() override {
        int ch = _stream.get();
        return ch != std::char_traits<char>::eof() ? ch : -1;
    }

    int read(char *buf, int len) override {
        _stream.read(buf, len);
        return _stream.gcount();
    }

    void close() {
        _stream.close();
    }

    size_t position() override {
        return static_cast<size_t>(_stream.tellg());
    }

    size_t length() override {
        auto pos = position();
        seek(0, SeekOrigin::End);
        auto len = position();
        seek(pos, SeekOrigin::Begin);
        return len;
    }

private:
    std::ifstream _stream;
};

} // namespace reone
