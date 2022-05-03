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

class FileInputStream : public IInputStream {
public:
    FileInputStream(const boost::filesystem::path &path, std::ios::openmode mode = std::ios::binary) :
        _stream(path, mode) {
    }

    size_t position() override {
        return static_cast<size_t>(_stream.tellg());
    }

    int readByte() override {
        int c = _stream.get();
        return !_stream.eof() ? c : -1;
    }

    int read(int length, ByteArray &outBytes) override {
        _stream.read(&outBytes[0], length);
        return _stream.gcount();
    }

private:
    boost::filesystem::ifstream _stream;
};

} // namespace reone
