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

#include "reone/system/binaryreader.h"
#include "reone/system/types.h"

namespace reone {

class IInputStream;

namespace resource {

class BinaryResourceReader : boost::noncopyable {
public:
    void load(IInputStream &in);

protected:
    boost::endian::order _endianess;

    std::unique_ptr<BinaryReader> _reader;

    size_t _size {0};

    BinaryResourceReader(boost::endian::order endianess = boost::endian::order::little) :
        _endianess(endianess) {
    }

    virtual void onLoad() = 0;

    void checkSignature(const std::string &expected);

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
    ByteArray readBytes(int count);
    ByteArray readBytes(size_t off, int count);

    inline std::vector<uint16_t> readUint16Array(int count) {
        return _reader->readUint16Array(count);
    }

    inline std::vector<uint32_t> readUint32Array(int count) {
        return _reader->readUint32Array(count);
    }

    inline std::vector<uint32_t> readUint32Array(size_t offset, int count) {
        return _reader->readUint32Array(offset, count);
    }

    inline std::vector<int32_t> readInt32Array(int count) {
        return _reader->readInt32Array(count);
    }

    inline std::vector<float> readFloatArray(int count) {
        return _reader->readFloatArray(count);
    }

    inline std::vector<float> readFloatArray(size_t offset, int count) {
        return _reader->readFloatArray(offset, count);
    }

private:
    void querySize();
};

} // namespace resource

} // namespace reone
