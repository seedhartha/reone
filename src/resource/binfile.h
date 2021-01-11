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

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "../common/streamreader.h"
#include "../common/types.h"

namespace reone {

namespace resource {

/**
 * Abstract class with utility methods for reading binary files.
 */
class BinaryFile {
public:
    void load(const std::shared_ptr<std::istream> &in);
    void load(const boost::filesystem::path &path);

protected:
    Endianess _endianess { Endianess::Little };
    boost::filesystem::path _path;
    std::shared_ptr<std::istream> _in;
    std::unique_ptr<StreamReader> _reader;
    size_t _size { 0 };

    BinaryFile(int signSize, const char *sign = 0);

    virtual void doLoad() = 0;

    size_t tell() const;
    void seek(size_t off);
    void ignore(int count);

    uint8_t readByte();
    uint16_t readUint16();
    uint32_t readUint32();
    uint64_t readUint64();
    int16_t readInt16();
    int32_t readInt32();
    float readFloat();
    std::string readCString(int len);
    std::string readCString(size_t off, int len);
    std::string readCStringAt(size_t off);
    std::string readString(int len);
    std::string readString(size_t off, int len);

    template <class T>
    std::vector<T> readArray(int count) {
        return _reader->getArray<T>(count);
    }

    template <class T>
    std::vector<T> readArray(uint32_t off, int count) {
        size_t pos = _reader->tell();
        _reader->seek(off);

        std::vector<T> result(_reader->getArray<T>(count));
        _reader->seek(pos);

        return std::move(result);
    }

private:
    int _signSize { 0 };
    ByteArray _sign;

    BinaryFile(const BinaryFile &) = delete;
    BinaryFile &operator=(const BinaryFile &) = delete;

    void load();
    void querySize();
    void checkSignature();
};

} // namespace resource

} // namespace reone
