/*
 * Copyright (c) 2020-2022 The reone project contributors
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

class FileInputStream : public IInputStream {
public:
    FileInputStream(const boost::filesystem::path &path, OpenMode mode = OpenMode::Text) :
        _stream(path, mode == OpenMode::Binary ? std::ios::binary : static_cast<std::ios::openmode>(0)) {
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
            throw std::invalid_argument("Unsupported origin: " + std::to_string(static_cast<int>(origin)));
        }
    }

    int readByte() override {
        int c = _stream.get();
        return !_stream.eof() ? c : -1;
    }

    int read(char *outData, int length) override {
        _stream.read(outData, length);
        return _stream.gcount();
    }

    void close() {
        _stream.close();
    }

    size_t position() override {
        return static_cast<size_t>(_stream.tellg());
    }

    bool eof() override {
        return _stream.eof();
    }

private:
    boost::filesystem::ifstream _stream;
};

} // namespace reone
